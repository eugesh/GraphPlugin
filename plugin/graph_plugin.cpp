#include "common.h"
#include "digitaldisplayboard.h"
#include "graph_plugin.h"
#include "graphmainwindow.h"
#include "graphpluginconfig.h"
#include "graphplugintablemodel.h"
#include "graphtableview.h"
#include "vectorindicatorsboard.h"

#include <QApplication>
#include <QDockWidget>
#include <QMainWindow>
#include <QTableView>
#include <QToolBar>

static const char* graphConfigsFolder = "configs/graphs";
static const char* pluginConfigsFolder = "configs/config";
static const char* SIConfigsFolder = "configs/si";
static const char* arrayIndicatorsJson = "configs/config/indicatorsBoard.json";

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

    if (m_tableModel)
        delete m_tableModel;

    if (m_tableView)
        delete m_tableView;

    if (m_tableDock)
        delete m_tableDock;

    if (m_boardDock)
        delete m_boardDock;
}

void GraphPlugin::setPacketSize(int size)
{
    m_tableModel->setPacketSize(size);
}

int GraphPlugin::packetSize() const
{
    return m_tableModel->packetSize();
}

QString GraphPlugin::aboutInfo()
{
    return QString("%1.%2.%3").arg(GRAPH_PLUGIN_VERSION_MAJOR).arg(GRAPH_PLUGIN_VERSION_MINOR).arg(GRAPH_PLUGIN_VERSION_PATCH);
}

bool GraphPlugin::loadJSONs()
{
    // Load SI units and prefixes
    auto prefPath = QString("%1/%2").arg(SIConfigsFolder).arg("prefixes.json");
    auto siPath = QString("%1/%2").arg(SIConfigsFolder).arg("aux-units-ru.json");
    m_config = new GraphPluginConfig(siPath, prefPath);

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
    loadTableJSON(QString("%1/%2").arg(graphConfigsFolder).arg("plugin_config.json"));

    // Read JSON for Score Board description
    loadSensorsMonitorJSON("");

    // Read JSON for Vector Indicators Board description
    loadVectorIndicatorsJSON("");

    restoreGraphPluginGeometry();

    return true;
}

bool GraphPlugin::restoreGraphPluginGeometry()
{
    //  Restore Geometry
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    auto allKeys = settings.allKeys();
    auto keys = settings.childKeys();
    auto groups = settings.childGroups();

    settings.beginGroup("MainWindow");

    auto geomData = settings.value("geometry").toByteArray();
    bool is_ok = m_mainWindow->restoreGeometry(geomData);

    if (m_mainWindow->isMaximized())
        m_mainWindow->setGeometry(QApplication::desktop()->availableGeometry(0));

    auto state = settings.value("windowState").toByteArray();
    is_ok = is_ok && m_mainWindow->restoreState(state);

    settings.endGroup();

    return is_ok;
}

bool GraphPlugin::saveGraphPluginGeometry()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    settings.beginGroup("MainWindow");

    auto geom = m_mainWindow->saveGeometry();
    settings.setValue("geometry", geom);
    auto state = m_mainWindow->saveState();
    settings.setValue("windowState", state);

    settings.endGroup();

    return true;
}

bool GraphPlugin::loadValuesJSON(const QString &pathToJSON)
{
    m_measValDescMap = loadConfigJSON(pathToJSON);

    return static_cast<bool>(m_measValDescMap.count());
}

bool GraphPlugin::loadConfig(const QString &pathToJSON)
{
    return true;
}

bool GraphPlugin::loadSI(const QString &pathToJSON)
{
    return true;
}

QStringList GraphPlugin::getValuesNames() const
{
    return m_measValDescMap.uniqueKeys();
}

QStringList GraphPlugin::getDescriptionsTr() const
{
    QStringList descs;

    for (auto valDesc : m_measValDescMap.values())
        descs << valDesc.desc_ru;

    return descs;
}

bool GraphPlugin::loadTableJSON(const QString &pathToJSON)
{
    m_tableModel = new GraphPluginTableModel(getDescriptionsTr(), getValuesNames(), m_synchMode, this);
    m_tableModel->setPacketSize(m_measValDescMap.size());
    m_tableView = new GraphTableView(m_mainWindow);
    m_tableView->setModel(m_tableModel);
    m_tableView->setConfig(m_config);
    m_tableView->setMeasValues(m_measValDescMap);
    m_tableDock = new QDockWidget(m_mainWindow);
    m_tableDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_tableDock->setWidget(m_tableView);
    m_tableDock->setObjectName("GraphTableViewDock");
    m_tableDock->toggleViewAction()->setText(tr("Таблица"));
    m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, m_tableDock);

    for (auto graphMainWindow : m_graphsMainWins.keys()) {
        connect(m_tableModel, &GraphPluginTableModel::packetFormed, m_graphsMainWins[graphMainWindow], &GraphMainWindow::addData);
        connect(m_tableModel, &GraphPluginTableModel::packetFormed, m_tableView, &QAbstractItemView::scrollToBottom);
    }

    connect (m_tableView, &GraphTableView::createNewGraph, this, &GraphPlugin::onAddNewPlot);
    connect (m_tableView, &GraphTableView::createNewVectorIndicator, this, &GraphPlugin::onAddNewVectorIndicator);
    connect(m_tableModel, &GraphPluginTableModel::packetFormed, m_vectorIndicatorsBoard, &VectorIndicatorsBoard::addData);

    return true;
}

bool GraphPlugin::loadSensorsMonitorJSON(const QString &pathToJSON)
{
    m_digitalBoard = new DigitalDisplayBoard();
    m_digitalBoard->setConfig(m_config);
    m_digitalBoard->setValuesDescriptions(m_measValDescMap);
    bool is_ok = m_digitalBoard->initFromJSON("");

    m_boardDock = new QDockWidget(m_mainWindow);
    m_boardDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_boardDock->setWidget(m_digitalBoard);
    m_boardDock->setObjectName("DigitalDisplayBoardDock");
    m_boardDock->toggleViewAction()->setText(tr("Табло"));
    m_mainWindow->addDockWidget(Qt::TopDockWidgetArea, m_boardDock);

    connect(m_tableModel, &GraphPluginTableModel::packetFormed, m_digitalBoard, &DigitalDisplayBoard::addData);

    return true;
}

bool GraphPlugin::loadVectorIndicatorsJSON(const QString &pathToJSON)
{
    m_vectorIndicatorsBoard = new VectorIndicatorsBoard();
    m_vectorIndicatorsBoard->setConfig(m_config);
    m_vectorIndicatorsBoard->setValuesDescriptions(m_measValDescMap);
    bool is_ok = m_vectorIndicatorsBoard->initFromJSON(arrayIndicatorsJson);

    m_vectorIndictorsDock = new QDockWidget(m_mainWindow);
    m_vectorIndictorsDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_vectorIndictorsDock->setWidget(m_vectorIndicatorsBoard);
    m_vectorIndictorsDock->setObjectName("VectorIndicatorsDock");
    m_vectorIndictorsDock->toggleViewAction()->setText(tr("Стрелочные индикаторы"));
    m_mainWindow->addDockWidget(Qt::TopDockWidgetArea, m_vectorIndictorsDock);

    connect(m_tableModel, &GraphPluginTableModel::packetFormed, m_vectorIndicatorsBoard, &VectorIndicatorsBoard::addData);

    return true;
}

/*bool GraphPlugin::saveGraphJSON(const QString &pathToJSON)
{

    return true;
}*/

bool GraphPlugin::loadGraphJSON(const QString &pathToJSON)
{
    QDockWidget *dock_widget = new QDockWidget(m_mainWindow);
    dock_widget->setAllowedAreas(Qt::AllDockWidgetAreas);
    GraphMainWindow *graphWindow = new GraphMainWindow(pathToJSON, m_mainWindow);
    graphWindow->setConfig(m_config);
    graphWindow->setValuesDescriptions(m_measValDescMap);

    connect(graphWindow, &GraphMainWindow::deleteMe, [=,this]() {
        m_graphsMainWins.remove(graphWindow->nameTr());
        m_mainWindow->removeDockWidget(dock_widget);
        m_graphsDocks.remove(graphWindow->nameTr());
        dock_widget->deleteLater();
    });

    dock_widget->setWidget(graphWindow);
    dock_widget->setObjectName(tr("%1%2").arg(graphWindow->objectName()).arg("Dock"));

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
    // m_dataMap[value.timestamp] = value.name;

    m_tableModel->appendValue(value);
}

QToolBar* GraphPlugin::toolBar() const
{
    return m_toolbar;
}

QList<QDockWidget*> GraphPlugin::dockWindows() const
{
    QList<QDockWidget*> list;

    list.append(m_graphsDocks.values());
    list.append(m_tableDock);
    list.append(m_boardDock);

    return list;
}

void GraphPlugin::setMainWindow(QMainWindow *mw)
{
    m_mainWindow = mw;
}

void GraphPlugin::onAddNewPlot(const QString &customPlotName, const GraphProperties &prop)
{
    if (m_graphsMainWins.contains(customPlotName)) {
        m_graphsMainWins[customPlotName]->addGraph(prop);
    } else {
        QDockWidget *dock_widget = new QDockWidget(m_mainWindow);
        dock_widget->setAllowedAreas(Qt::AllDockWidgetAreas);

        GraphMainWindow *graphWindow = new GraphMainWindow(customPlotName, prop, m_mainWindow);
        graphWindow->setConfig(m_config);
        graphWindow->setValuesDescriptions(m_measValDescMap);

        dock_widget->setWidget(graphWindow);

        m_graphsDocks.insert(graphWindow->nameTr(), dock_widget);
        m_graphsMainWins.insert(graphWindow->nameTr(), graphWindow);

        m_mainWindow->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dock_widget);

        connect(m_tableModel, &GraphPluginTableModel::packetFormed, graphWindow, &GraphMainWindow::addData);
    }
}

void GraphPlugin::onAddNewVectorIndicator(const QString &customPlotName, const GraphProperties &prop)
{
    m_vectorIndicatorsBoard->addNewIndicator(prop);
}
