#include "graph_plugin.h"
#include "graphmainwindow.h"
#include "graphplugintablemodel.h"

#include <QDockWidget>
#include <QMainWindow>
#include <QToolBar>

static const char* graphConfigsFolder = "graphs";
static const char* pluginConfigsFolder = "config";
static const char* SIConfigsFolder = "si";

struct MeasuredValueDescrition {
    QString name;
    QString desc;
    QString unit;
    QString unit_rus;
};

GraphPlugin::GraphPlugin(QMainWindow *mw) : QObject(mw)
  , m_mainWindow(mw)
{
    // Read JSON with input data description (measuring values types)

    // Read JSON with ToolBar description (content, icons)
    m_toolbar = new QToolBar(mw);

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
    m_tableModel = new GraphPluginTableModel(this);

    for (auto graphMainWindow : m_graphsMainWins)
        connect(m_tableModel, &GraphPluginTableModel::packetFormed, graphMainWindow, &GraphMainWindow::addData);
}

GraphPlugin::~GraphPlugin()
{

}

bool GraphPlugin::loadValuesJSON(const QString &pathToJSON)
{
    QFile loadFile(pathToJSON);

    MeasuredValueDescrition mvdesc_struct;

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
        mvdesc_struct.unit = valueObject["measure_unit"].toString();
        mvdesc_struct.unit_rus = valueObject["measure_unit_rus"].toString();
        m_mvdesc_struct.push_back(mvdesc_struct);
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

bool GraphPlugin::loadGraphJSON(const QString &pathToJSON)
{
    QDockWidget *dock_widget = new QDockWidget(m_mainWindow);

    GraphMainWindow *graphWindow = new GraphMainWindow(pathToJSON, m_mainWindow);

    dock_widget->setWidget(graphWindow);

    m_graphsDocks.append(dock_widget);
    m_graphsMainWins.append(graphWindow);

    return true;
}

QString GraphPlugin::echo(const QString &message)
{
    return message;
}

void GraphPlugin::addData(const MeasuredValue &value)
{
    m_dataMap[value.timestamp] = value.name;
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
