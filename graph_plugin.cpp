#include "common.h"
#include "graph_plugin.h"
#include "graphmainwindow.h"
#include "graphpluginconfig.h"
#include "graphplugintablemodel.h"
#include "graphtableview.h"

#include <QDockWidget>
#include <QMainWindow>
#include <QTableView>
#include <QToolBar>

static const char* graphConfigsFolder = "configs/graphs";
static const char* pluginConfigsFolder = "configs/config";
static const char* SIConfigsFolder = "configs/si";

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

    // Read JSON for Score Board description

    // Read JSON for Table and it's model
    loadTableJSON(QString("%1/%2").arg(graphConfigsFolder).arg("plugin_config.json"));

    return true;
}

bool GraphPlugin::loadValuesJSON(const QString &pathToJSON)
{
    QFile loadFile(pathToJSON);

    MeasuredValueDescription mvdesc_struct;

    if (! loadFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Input file " << pathToJSON << " wasn't opened on read";
        return false;
    }

    QByteArray loadData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(loadData));

    QJsonArray valuesArray = loadDoc.object()["values"].toArray();

    for (int i = 0; i < valuesArray.size(); ++i) {
        QJsonObject valueObject = valuesArray[i].toObject();
        mvdesc_struct.name = valueObject["name"].toString();
        mvdesc_struct.desc = valueObject["description"].toString();
        mvdesc_struct.physQuant = valueObject["physicalQuantity"].toString();
        mvdesc_struct.unit = valueObject["measure_unit"].toString();
        mvdesc_struct.unit_rus = valueObject["measure_unit_rus"].toString();
        m_measValDescMap.insert(mvdesc_struct.name, mvdesc_struct);
    }

    return true;
}

bool GraphPlugin::loadConfig(const QString &pathToJSON)
{
    return true;
}

bool GraphPlugin::loadSI(const QString &pathToJSON)
{
    return true;
}

bool GraphPlugin::loadTableJSON(const QString &pathToJSON)
{
    m_tableModel = new GraphPluginTableModel(this);
    m_tableView = new GraphTableView(m_mainWindow);
    m_tableView->setModel(m_tableModel);
    m_tableView->setConfig(m_config);
    m_tableView->setMeasValues(m_measValDescMap);
    m_tableDock = new QDockWidget(m_mainWindow);
    m_tableDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_tableDock->setWidget(m_tableView);

    m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, m_tableDock);

    for (auto graphMainWindow : m_graphsMainWins.keys())
        connect(m_tableModel, &GraphPluginTableModel::packetFormed, m_graphsMainWins[graphMainWindow], &GraphMainWindow::addData);

    connect (m_tableView, &GraphTableView::createNewGraph, this, &GraphPlugin::onAddNewPlot);

    return true;
}

bool GraphPlugin::saveGraphJSON(const QString &pathToJSON)
{

    return true;
}

bool GraphPlugin::loadGraphJSON(const QString &pathToJSON)
{
    QDockWidget *dock_widget = new QDockWidget(m_mainWindow);
    dock_widget->setAllowedAreas(Qt::AllDockWidgetAreas);
    GraphMainWindow *graphWindow = new GraphMainWindow(pathToJSON, m_mainWindow);

    dock_widget->setWidget(graphWindow);

    m_graphsDocks.append(dock_widget);
    m_graphsMainWins.insert(graphWindow->objectName(), graphWindow);

    m_mainWindow->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dock_widget);

    return true;
}

QString GraphPlugin::echo(const QString &message)
{
    return message;
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

    list.append(m_graphsDocks);
    list.append(m_tableDock);
    list.append(m_scoreBoardDock);

    return list;
}

void GraphPlugin::setMainWindow(QMainWindow *mw)
{
    m_mainWindow = mw;
}

void GraphPlugin::onAddNewPlot(const QString &customPlotName, const GraphProperties &prop)
{
    if (m_graphsMainWins.contains(customPlotName)) {
        m_graphsMainWins[prop.name]->addGraph(prop);
    } else {
        QDockWidget *dock_widget = new QDockWidget(m_mainWindow);
        dock_widget->setAllowedAreas(Qt::AllDockWidgetAreas);

        GraphMainWindow *graphWindow = new GraphMainWindow(customPlotName, prop, m_mainWindow);

        dock_widget->setWidget(graphWindow);

        m_graphsDocks.append(dock_widget);
        m_graphsMainWins.insert(graphWindow->objectName(), graphWindow);

        m_mainWindow->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dock_widget);

        connect(m_tableModel, &GraphPluginTableModel::packetFormed, graphWindow, &GraphMainWindow::addData);
    }
}
