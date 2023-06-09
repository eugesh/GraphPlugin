#include "common.h"
#include "digitaldisplayboard.h"
#include "graph_plugin.h"
#include "graphmainwindow.h"
#include "graphpluginconfig.h"
#include "graphtablemodel.h"
#include "graphtableview.h"
#include "vectorindicatorsboard.h"

#include <QApplication>
#include <QDir>
#include <QDockWidget>
#include <QMainWindow>
#include <QTableView>
#include <QToolBar>

using namespace Graph;

//   configs/{[deviceName]|si}/{config|graph}
static const char* allConfigsFolder = "configs";
/*static const char* adcpConfigsFolder = "adcp";
static const char* dvlConfigsFolder = "dvl";
static const char* gpsConfigsFolder = "gps";*/
static const char* graphConfigsFolder = "graphs";
static const char* pluginConfigsFolder = "config";
static const char* SIConfigsFolder = "configs/si";
static const char* arrayIndicatorsJson = "indicatorsBoard.json";

/**
 * @brief GraphPlugin::GraphPlugin
 * Implementation of \link GraphInterface \endlink.
 * @param mw pointer to QMainWindow
 */
GraphPlugin::GraphPlugin(QMainWindow *mw) : QObject(mw)
  , m_mainWindow(mw)
{
    // Read JSON with input data description (measuring values types)

    // Read JSON with ToolBar description (content, icons)
    m_toolbar = new QToolBar(mw);

    // Table

}

GraphPlugin::~GraphPlugin()
{
    for (auto dock : m_graphsMainWins)
        if (dock)
            delete dock;

    for (auto dock : m_graphsDocks)
        if (dock)
            delete dock;

    if (m_config)
        delete m_config;

    if (!m_tableModelMap.isEmpty())
        for (auto tableModel : m_tableModelMap)
            delete tableModel;

    if (!m_tableViewMap.isEmpty())
        for (auto tableView : m_tableViewMap)
            delete tableView;

    if (!m_tableDockMap.isEmpty())
        for (auto tableDock : m_tableDockMap)
            delete tableDock;

    if (m_boardDock)
        delete m_boardDock;
}

void GraphPlugin::setUpdateable(bool isUpdateable)
{
    for (auto graphWin : m_graphsMainWins)
        graphWin->setUpdateAble(isUpdateable);
}

void GraphPlugin::clearAll()
{
    for (auto model : m_tableModelMap)
        model->clear();

    for (auto graphWin : m_graphsMainWins)
        graphWin->clearAll();
}

void GraphPlugin::setPacketSize(int size, const QString tableName)
{
    if (size <= 0)
        return;

    if (m_tableModelMap.value(tableName))
        m_tableModelMap.value(tableName)->setPacketSize(size);
}

int GraphPlugin::packetSize(const QString tableName) const
{
    return m_tableModelMap[tableName]->packetSize();
}

QString GraphPlugin::aboutInfo()
{
    return QString("%1.%2.%3").arg(GRAPH_PLUGIN_VERSION_MAJOR).arg(GRAPH_PLUGIN_VERSION_MINOR).arg(GRAPH_PLUGIN_VERSION_PATCH);
}

/**
 * @brief GraphPlugin::loadJSONs
 * Loads all configurations saved in JSON files.
 * @param subdirsNames names of folders to llok for JSON files.
 * @return true|false
 */
bool GraphPlugin::loadJSONs(QStringList subdirsNames)
{
    // Load SI units and prefixes
    auto prefPath = QString("%1/%2").arg(SIConfigsFolder).arg("prefixes.json");
    auto siPath = QString("%1/%2").arg(SIConfigsFolder).arg("aux-units-ru.json");
    m_config = new GraphPluginConfig(siPath, prefPath);

    if (!subdirsNames.isEmpty()) {
        auto appPath = QCoreApplication::applicationDirPath();
        QDir allConfigs(appPath);
#ifdef Q_OS_WIN
        if (appPath.endsWith("debug") || appPath.endsWith("release"))
            allConfigs.cd("..");
#endif
        allConfigs.cd(allConfigsFolder);
        const QStringList entries = allConfigs.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &dirName : entries) {
            qDebug() << dirName;
            if (subdirsNames.contains(dirName, Qt::CaseInsensitive)) {

                // Load used in project values
                auto path = QString("%1/%2/%3/%4").arg(allConfigsFolder).arg(dirName).arg(pluginConfigsFolder).arg("plugin_config.json");
                loadValuesJSON(path, dirName.toUpper());

                // Read all JSON files with Graph Plot Windows descriptions
                auto graphsDirName = QString("%1/%2/%3").arg(allConfigsFolder).arg(dirName).arg(graphConfigsFolder);
                QDir graphConfigsDir(graphsDirName);
                for (auto name : graphConfigsDir.entryList({"*.json"})) {
                    auto path = QString("%1/%2").arg(graphsDirName).arg(name);
                    loadGraphJSON(path);
                }

                // Read JSON for Table and it's model
                loadTableJSON(path, dirName.toUpper());

                // Read JSON for Vector Indicators Board description
                auto indPath = QString("%1/%2/%3/%4").arg(allConfigsFolder).arg(dirName).arg(pluginConfigsFolder).arg(arrayIndicatorsJson);
                loadVectorIndicatorsJSON(indPath);

                // Read JSON for Score Board description
                loadSensorsMonitorJSON("", dirName.toUpper());
            }
        }
        restoreGraphPluginGeometry(subdirsNames.join("_"));
    } else {
        // Load SI units and prefixes
        // auto prefPath = QString("%1/%2").arg(SIConfigsFolder).arg("prefixes.json");
        // auto siPath = QString("%1/%2").arg(SIConfigsFolder).arg("aux-units-ru.json");
        // m_config = new GraphPluginConfig(siPath, prefPath);

        // Load used in project values
        auto path = QString("%1/%2").arg(pluginConfigsFolder).arg("plugin_config.json");
        loadValuesJSON(path);

        // Read all JSON files with Graph Plot Windows descriptions
        QDir graphConfigsDir(graphConfigsFolder);
        for (auto name : graphConfigsDir.entryList({"*.json"})) {
            auto path = QString("%1/%2").arg(graphConfigsFolder).arg(name);
            loadGraphJSON(path);
        }

        // Read JSON for Table and it's model
        loadTableJSON(QString("%1/%2").arg(pluginConfigsFolder).arg("plugin_config.json"));

        // Read JSON for Score Board description
        loadSensorsMonitorJSON("");

        // Read JSON for Vector Indicators Board description
        loadVectorIndicatorsJSON(arrayIndicatorsJson);

        restoreGraphPluginGeometry();
    }

    return true;
}

bool GraphPlugin::restoreGraphPluginGeometry(const QString &suffix)
{
    //  Restore Geometry
    QDir lay_dir(m_layoutIniFile);
    QScopedPointer<QSettings> settings;
    auto pwd = QDir::currentPath();

    if (lay_dir.exists(pwd + "/" + m_layoutIniFile)) {
        settings.reset(new QSettings(pwd + "/" + m_layoutIniFile, QSettings::IniFormat));
    } else {
        settings.reset(new QSettings(QApplication::organizationName(), QApplication::applicationName()));
    }
    if (settings.isNull())
        return false;

    settings->beginGroup("MainWindow" + suffix);

    auto geomData = settings->value("geometry").toByteArray();
    bool is_ok = m_mainWindow->restoreGeometry(geomData);

    if (!is_ok)
        qWarning() << "Error: failed to restore GraphPlugin geometry";

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (m_mainWindow->isMaximized()) {
        // m_mainWindow->setGeometry(qApp->screenAt(QPoint(0,0))->availableGeometry()); // Valid but not tested. Buildable with Qt5 and Qt6.
        m_mainWindow->setGeometry(QApplication::desktop()->availableGeometry(0));
    } else {
        m_mainWindow->showMaximized();
    }
#endif

    auto state = settings->value("windowState").toByteArray();
    bool is_ok2 = m_mainWindow->restoreState(state);
    if (!is_ok2)
        qWarning() << "Error: failed to restore GraphPlugin state";

    if (m_mainWindow->isFullScreen())
        m_mainWindow->showMaximized();

    settings->endGroup();

    return is_ok;
}

bool GraphPlugin::saveGraphPluginGeometry(const QString &suffix)
{
    QDir lay_dir(m_layoutIniFile);
    QSettings *settings = nullptr;
    auto pwd = QDir::currentPath();

    if (lay_dir.exists(pwd + "/" + m_layoutIniFile)) {
        settings = new QSettings(pwd + "/" + m_layoutIniFile, QSettings::IniFormat);
    } else {
        settings = new QSettings(pwd + "/" + QApplication::organizationName(), QApplication::applicationName());
    }
    if (!settings)
        return false;

    settings->beginGroup("MainWindow" + suffix);

    auto geom = m_mainWindow->saveGeometry();
    settings->setValue("geometry", geom);
    auto state = m_mainWindow->saveState();
    settings->setValue("windowState", state);

    settings->endGroup();

    delete settings;

    return true;
}

/**
 * @brief GraphPlugin::loadValuesJSON
 * Reads JSON file with Measured Values descriptions for some history table.
 * @param pathToJSON path to JSON (e.g. "plugin_config.json").
 * @param tableName name of the history table.
 * @return true|false.
 */
bool GraphPlugin::loadValuesJSON(const QString &pathToJSON, const QString &tableName)
{
    auto map = Graph::loadConfigJSON(pathToJSON);
    // m_measValDescMap.unite(map);
    m_measValDescMap.insert(tableName, map);

    for (auto key : map.keys())
        m_tableMeasValNames.insert(tableName, key);

    return static_cast<bool>(map.count());
}

bool GraphPlugin::loadConfig(const QString &pathToJSON)
{
    return true;
}

bool GraphPlugin::loadSI(const QString &pathToJSON)
{
    return true;
}

/**
 * @brief GraphPlugin::getValuesNames
 * @param tableName name of the history table.
 * @return list of Measured Values names.
 */
QStringList GraphPlugin::getValuesNames(const QString &tableName) const
{
    return m_measValDescMap.value(tableName).uniqueKeys();
}

/**
 * @brief GraphPlugin::getDescriptionsTr
 * @param tableName name of history table.
 * @return список описаний на текущем языке.
 */
QStringList GraphPlugin::getDescriptionsTr(const QString &tableName) const
{
    QStringList descs;

    for (auto valDesc : m_measValDescMap.value(tableName).values())
        descs << valDesc.desc_ru;

    return descs;
}

/**
 * @brief GraphPlugin::loadTableJSON
 * Initialization of history table's model/view (see MVC).
 * @param pathToJSON unused.
 * @param tableName name of table.
 * @return true|false.
 */
bool GraphPlugin::loadTableJSON(const QString &pathToJSON, const QString &tableName)
{
    // GraphPluginTableModel *tableModel = new GraphPluginTableModel(getDescriptionsTr(tableName), getValuesNames(tableName), m_synchMode, this);
    GraphTableModel *tableModel = new GraphTableModel(getValuesNames(tableName), getValuesNames(tableName), m_synchMode, this);
    tableModel->setPacketSize(m_measValDescMap.value(tableName).size());
    GraphTableView *tableView = new GraphTableView(m_mainWindow);
    tableView->setModel(tableModel);
    tableView->setConfig(m_config);
    tableView->setMeasValues(m_measValDescMap.value(tableName));
    QDockWidget *tableDock = new QDockWidget(m_mainWindow);
    tableDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    tableDock->setWidget(tableView);
    tableDock->setObjectName("GraphTableViewDock");
    tableDock->toggleViewAction()->setText(tr("Таблица ") + tableName);
    m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, tableDock);

    m_tableModelMap.insert(tableName, tableModel);
    m_tableViewMap.insert(tableName, tableView);
    m_tableDockMap.insert(tableName, tableDock);

    for (auto mwName : m_graphsMainWins.keys()) {
        connect(tableModel, &GraphTableModel::packetFormed, m_graphsMainWins[mwName], &GraphMainWindow::addData);
        connect(tableModel, &GraphTableModel::packetFormed, m_graphsMainWins[mwName], &GraphMainWindow::add2dData);
        connect(tableModel, &GraphTableModel::packetFormed, tableView, &QAbstractItemView::scrollToBottom);
    }

    connect(tableView, &GraphTableView::createNewGraph, this, &GraphPlugin::onAddNewPlot);
    connect(tableView, &GraphTableView::createNewVectorIndicator, this, &GraphPlugin::onAddNewVectorIndicator);

    if (m_vectorIndicatorsBoard)
        connect(tableModel, &GraphTableModel::packetFormed, m_vectorIndicatorsBoard, &VectorIndicatorsBoard::addData);

    return true;
}

/**
 * @brief GraphPlugin::loadSensorsMonitorJSON
 * Initialization of LCD indicators board.
 * @param pathToJSON unused.
 * @param tableName name of history table.
 * @return true|false.
 */
bool GraphPlugin::loadSensorsMonitorJSON(const QString &pathToJSON, const QString &tableName)
{
    m_digitalBoard = new DigitalDisplayBoard();
    m_digitalBoard->setConfig(m_config);
    QMultiMap<QString, Graph::MeasuredValueDescription> allDescMap;
    if (tableName.isEmpty()) {
        for (auto map : m_measValDescMap)
            allDescMap.unite(map);
    } else {
        allDescMap = m_measValDescMap.value(tableName);
    }
    m_digitalBoard->setValuesDescriptions(allDescMap);
    bool is_ok = m_digitalBoard->initFromJSON("");

    m_boardDock = new QDockWidget(m_mainWindow);
    m_boardDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_boardDock->setWidget(m_digitalBoard);
    m_boardDock->setObjectName("DigitalDisplayBoardDock");
    m_boardDock->toggleViewAction()->setText(tr("Цифровые индикаторы ") + tableName);
    m_mainWindow->addDockWidget(Qt::TopDockWidgetArea, m_boardDock);

    for (auto tableModel : m_tableModelMap.values())
        connect(tableModel, &GraphTableModel::packetFormed, m_digitalBoard, &DigitalDisplayBoard::addData);

    return true;
}

/**
 * @brief GraphPlugin::loadVectorIndicatorsJSON
 * @param pathToJSON path to JSON configuration file ("indicatorsBoard.json").
 * @return true|false.
 */
bool GraphPlugin::loadVectorIndicatorsJSON(const QString &pathToJSON)
{
    QFileInfo info(pathToJSON);
    if (!info.exists())
        return false;

    m_vectorIndicatorsBoard = new VectorIndicatorsBoard();

    m_vectorIndicatorsBoard->setConfig(m_config);
    QMultiMap<QString, Graph::MeasuredValueDescription> allDescMap;
    for (auto map : m_measValDescMap)
        allDescMap.unite(map);
    m_vectorIndicatorsBoard->setValuesDescriptions(allDescMap);
    bool is_ok = m_vectorIndicatorsBoard->initFromJSON(pathToJSON);

    if (!is_ok) {
        delete m_vectorIndicatorsBoard;
        m_vectorIndicatorsBoard = nullptr;
        return false;
    }

    m_vectorIndictorsDock = new QDockWidget(m_mainWindow);
    m_vectorIndictorsDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_vectorIndictorsDock->setWidget(m_vectorIndicatorsBoard);
    m_vectorIndictorsDock->setObjectName("VectorIndicatorsDock");
    m_vectorIndictorsDock->toggleViewAction()->setText(tr("Стрелочные индикаторы"));
    //m_vectorIndicatorsBoard->setAutoDisableOnIdle(1000);

    m_mainWindow->addDockWidget(Qt::TopDockWidgetArea, m_vectorIndictorsDock);

    for (auto tableModel : m_tableModelMap.values())
        connect(tableModel, &GraphTableModel::packetFormed, m_vectorIndicatorsBoard, &VectorIndicatorsBoard::addData);

    return true;
}

/**
 * @brief GraphPlugin::loadGraphJSON
 * @param pathToJSON path to JSON file (e.g. "configs/devicename/graphs/graphname.json").
 * @return true|false.
 */
bool GraphPlugin::loadGraphJSON(const QString &pathToJSON)
{
    QDockWidget *dock_widget = new QDockWidget(m_mainWindow);
    dock_widget->setAllowedAreas(Qt::AllDockWidgetAreas);
    GraphMainWindow *graphWindow = new GraphMainWindow(pathToJSON, m_mainWindow);
    graphWindow->setConfig(m_config);
    QMultiMap<QString, Graph::MeasuredValueDescription> allDescMap;
    for (auto map : m_measValDescMap)
        allDescMap.unite(map);
    graphWindow->setValuesDescriptions(allDescMap);

    connect(graphWindow, &GraphMainWindow::deleteMe, [=, this]() {
        m_graphsMainWins.remove(graphWindow->nameTr());
        m_mainWindow->removeDockWidget(dock_widget);
        m_graphsDocks.remove(graphWindow->nameTr());
        dock_widget->deleteLater();
    });

    dock_widget->setWidget(graphWindow);
    dock_widget->setObjectName(tr("%1%2").arg(graphWindow->objectName(), "Dock"));

    m_graphsDocks.insert(graphWindow->nameTr(), dock_widget);
    m_graphsMainWins.insert(graphWindow->nameTr(), graphWindow);

    dock_widget->toggleViewAction()->setText(graphWindow->nameTr());

    m_mainWindow->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dock_widget);

    return true;
}

void GraphPlugin::setMode(GraphPluginMode mode)
{
    m_synchMode = mode;
}

void GraphPlugin::addData(const MeasuredValue &value)
{
    for (auto tableName : m_tableModelMap.keys()) {
        auto map = m_measValDescMap.value(tableName);
        if (map.contains(value.name)) {
            m_tableModelMap.value(tableName)->appendValue(value);
        }
    }

    /*for (auto tableModel : m_tableModelMap) {
        tableModel->appendValue(value);
    }*/
    // m_tableModel->appendValue(value);
}

QToolBar* GraphPlugin::toolBar() const
{
    return m_toolbar;
}

QList<QDockWidget*> GraphPlugin::dockWindows() const
{
    QList<QDockWidget*> list;

    list.append(m_graphsDocks.values());
    list.append(m_tableDockMap.values());
    list.append(m_boardDock);

    return list;
}

void GraphPlugin::setMainWindow(QMainWindow *mw)
{
    m_mainWindow = mw;
}

/**
 * @brief GraphPlugin::onAddNewPlot
 * @param customPlotName name of custom plot area
 * @param prop graph properties \link GraphProperties \endlink
 */
void GraphPlugin::onAddNewPlot(const QString &customPlotName, const Graph::GraphProperties &prop)
{
    if (m_graphsMainWins.contains(customPlotName)) {
        m_graphsMainWins[customPlotName]->addGraph(prop);
    } else {
        QDockWidget *dock_widget = new QDockWidget(m_mainWindow);
        dock_widget->setAllowedAreas(Qt::AllDockWidgetAreas);

        GraphMainWindow *graphWindow = new GraphMainWindow(customPlotName, prop, m_mainWindow);
        graphWindow->setConfig(m_config);
        QMultiMap<QString, Graph::MeasuredValueDescription> allDescMap;
        for (auto &map : m_measValDescMap)
            allDescMap.unite(map);
        graphWindow->setValuesDescriptions(allDescMap);

        dock_widget->setWidget(graphWindow);

        m_graphsDocks.insert(graphWindow->nameTr(), dock_widget);
        m_graphsMainWins.insert(graphWindow->nameTr(), graphWindow);

        m_mainWindow->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dock_widget);

        for (auto &tableModel : m_tableModelMap)
            connect(tableModel, &GraphTableModel::packetFormed, graphWindow, &GraphMainWindow::addData);
    }
}

/**
 * @brief GraphPlugin::onAddNewVectorIndicator
 * @param customPlotName
 * @param prop
 */
void GraphPlugin::onAddNewVectorIndicator(const QString &customPlotName, const Graph::GraphProperties &prop)
{
    if (m_vectorIndicatorsBoard)
        m_vectorIndicatorsBoard->addNewIndicator(prop);
}
