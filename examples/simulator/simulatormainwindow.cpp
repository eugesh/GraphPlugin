#include "channelconfigurationdialog.h"
#include "channeltuner.h"
#include "graph_interface.h"
#include "graphpluginconfig.h"
#include "simulatormainwindow.h"
#include "ui_simulatormainwindow.h"

#include <QDateTime>
#include <QDebug>
#include <QDockWidget>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QEventLoop>
#include <QFormLayout>
#include <QPluginLoader>
#include <cmath>

#define AutoDisconnect(l) \
    QSharedPointer<QMetaObject::Connection> l = QSharedPointer<QMetaObject::Connection>(\
        new QMetaObject::Connection(), \
        [](QMetaObject::Connection * conn) { /*QSharedPointer сам по себе не производит отключения при удалении*/ \
            QObject::disconnect(*conn);\
        }\
    ); *l //-- Use AutoDisconnect(conn1) = connect(....);

static const char* graphConfigsFolder = "configs/graphs";
static const char* pluginConfigsFolder = "configs/config";
static const char* SIConfigsFolder = "configs/si";

SimulatorMainWindow::SimulatorMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButtonAddChannel, &QPushButton::pressed, this, &SimulatorMainWindow::addChannel);
    connect(ui->pushButtonRemoveChannel, &QPushButton::pressed, this, &SimulatorMainWindow::removeChannel);
    connect(ui->pushButtonRun, &QPushButton::pressed, this, &SimulatorMainWindow::onRun);
    connect(ui->pushButtonStop, &QPushButton::pressed, this, &SimulatorMainWindow::onStop);
    connect(ui->pushButtonConfigure, &QPushButton::pressed, this, &SimulatorMainWindow::onConfigure);

    m_formLayout = new QFormLayout(this);
    ui->widget->setLayout(m_formLayout);

    loadGraphPlugin();

    // Load SI units and prefixes
    auto prefPath = QString("%1/%2").arg(SIConfigsFolder).arg("prefixes.json");
    auto siPath = QString("%1/%2").arg(SIConfigsFolder).arg("aux-units-ru.json");
    auto valuesPath = QString("%1/%2").arg(pluginConfigsFolder).arg("plugin_config.json");

    m_globalConfig = new GraphPluginConfig(siPath, prefPath);

    m_currentConfig = loadConfigJSON(valuesPath);

    fillGUI();

    srand(time(0));
}

SimulatorMainWindow::~SimulatorMainWindow()
{
    if (m_graphInterface) {
        m_graphInterface->saveGraphPluginGeometry();
    }

    unloadGraphPlugin();

    delete ui;
}

bool SimulatorMainWindow::loadGraphPlugin()
{
    QDir pluginsDir(QCoreApplication::applicationDirPath());
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");
    const QStringList entries = pluginsDir.entryList(QDir::Files);
    for (const QString &fileName : entries) {
        m_graphPluginLoader = new QPluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = m_graphPluginLoader->instance();
        if (plugin) {
            m_graphInterface = qobject_cast<GraphInterface *>(plugin);

            m_graphInterface->setMainWindow(this);

            m_graphInterface->loadJSONs();

            connect(this, &SimulatorMainWindow::newData, [&](const MeasuredValue &val) {
                m_graphInterface->addData(val);
            });

            if (m_graphInterface) {
                return true;
            }

            qWarning() << "Graph Plugin wasn't loaded!";
            m_graphPluginLoader->unload();
        }
    }

    return false;
}

void SimulatorMainWindow::fillGUI()
{
    for (auto val : m_currentConfig.values()) {
        addChannel();
        channelTunerAt(m_channelNum - 1)->setCurrentPhysicalValue(val.physQuant);
        channelTunerAt(m_channelNum - 1)->setCurrentMeasurementUnit(val.unit_rus);
        channelTunerAt(m_channelNum - 1)->setName(val.name);
        channelTunerAt(m_channelNum - 1)->setDescription(val.desc_ru);
    }
}

bool SimulatorMainWindow::unloadGraphPlugin()
{
    if (m_graphPluginLoader && m_graphPluginLoader->isLoaded()) {
        // auto docks = m_graphInterface->dockWindows();
        for (QDockWidget *widget :  m_graphInterface->dockWindows()) {
            /*QEventLoop _loop;
            AutoDisconnect(conn) = connect(widget, &QDockWidget::destroyed, [&_loop]() {
                _loop.exit();
            });*/
            removeDockWidget(widget);
            // delete widget;
            // _loop.exec();
        }

        if (!m_graphPluginLoader->unload()) {
            qWarning() << "Can't unload plugin: " << m_graphPluginLoader->errorString();
            return false;
        } else {
            delete m_graphPluginLoader;
            m_graphPluginLoader = nullptr;
            return true;
        }
    } else {
        return false;
    }
}

// Read JSON to propose user comboboxes with lists of SI units
bool SimulatorMainWindow::readConfigJSON(const QString &path)
{

    return true;
}

// Write JSONs after configuration process is finished
bool SimulatorMainWindow::writeConfigJSON(const QString &pathToJSON) const
{
    QFile writeFile(pathToJSON);

    if (! writeFile.open(QIODevice::WriteOnly)) {
        qCritical() << "Output file " << pathToJSON << " wasn't opened on write";
        return false;
    }

    QJsonObject docObject;
    QJsonArray valuesArray;

    for (int i = 0; i < m_formLayout->rowCount(); ++i) {
        auto widg = static_cast<ChannelTuner*> (m_formLayout->itemAt(i)->widget());
        if (widg) {
            QJsonObject valueObject;
            auto physValName = widg->physicalValueName();
            auto measUnitName = widg->measurementUnitName();
            auto measUnitNameTr = widg->measurementUnitNameTr();

            QList<MeasUnit> units = m_globalConfig->measurementUnits(physValName);
            // Find out selected unit in units list
            MeasUnit selectedUnit;
            for (auto unit : units) {
                if (unit["name"] == measUnitName)
                    selectedUnit = unit;
            }
            valueObject["name"] = widg->name();
            valueObject["description"] = "";
            valueObject["description_ru"] = widg->description();
            valueObject["physicalQuantity"] = physValName;
            valueObject["measure_unit"] = measUnitName;
            valueObject["measure_unit_rus"] = measUnitNameTr;
            valueObject["symbol"] = selectedUnit["symbol"].toString();
            valueObject["symbol_rus"] = selectedUnit["symbol_ru"].toString();

            valuesArray.append(valueObject);
        }
    }

    docObject["values"] = valuesArray;
    QJsonDocument saveDoc(docObject);
    writeFile.write(saveDoc.toJson());

    return true;
}

bool SimulatorMainWindow::writeGraphJSONs() const
{
    for (int i = 0; i < m_formLayout->rowCount(); ++i) {
        if (channelTunerAt(i)->isUseStandardPlot()) {

        }
    }
}

bool SimulatorMainWindow::writeGraphJSON(const QString &path) const
{

    return true;
}

void SimulatorMainWindow::enableConfigure(bool isEnabled)
{
    m_enableConfigure = isEnabled;

    ui->pushButtonConfigure->setEnabled(isEnabled);
    ui->pushButtonRun->setEnabled(!isEnabled);
    ui->pushButtonStop->setEnabled(!isEnabled);
}

void SimulatorMainWindow::addChannel()
{
    enableConfigure(true);

    m_channelNum++;

    ChannelTuner *ct = new ChannelTuner(m_channelNum, m_globalConfig);

    m_formLayout->addRow(ct);
}

ChannelTuner *SimulatorMainWindow::channelTunerAt(int index) const
{
    auto widg = static_cast<ChannelTuner*> (m_formLayout->itemAt(index)->widget());

    return widg;
}

void SimulatorMainWindow::removeChannel()
{
    enableConfigure(true);

    int lastRow = m_formLayout->count() - 1;

    if (!lastRow)
        return;

    m_channelNum--;

    m_formLayout->removeRow(lastRow);
}

QVector<MeasuredValue> SimulatorMainWindow::allCurrentValues() const
{
    QVector<MeasuredValue> vec;

    uint64_t timestamp = QDateTime::currentMSecsSinceEpoch();

    for (int i = 0; i < m_formLayout->count(); ++i) {
        MeasuredValue val;
        auto widg = static_cast<ChannelTuner*> (m_formLayout->itemAt(i)->widget());
        if (widg) {
            val.value = widg->randomValue();
            val.timestamp = timestamp;
            val.channel = 1;
            val.name = widg->name();
            vec << val;
        }
    }

    return vec;
}

MeasuredValue SimulatorMainWindow::currentValue(const QString &name) const
{
    MeasuredValue val;

    val.timestamp = QDateTime::currentMSecsSinceEpoch();

    return val;
}

void SimulatorMainWindow::onRun()
{
    if (m_enableConfigure || !m_graphPluginLoader) {
        // ToDo: QMessage here
        return;
    }

    QEventLoop _loop;

    m_state = RUN;

    while (m_state == RUN) {
        AutoDisconnect(conn) = connect(&m_greqTimer, &QTimer::timeout, [&_loop]() {
            _loop.exit();
        });
        m_greqTimer.start(1000.0 / ui->spinBox->value());
        _loop.exec();
        for (auto val : allCurrentValues())
            emit newData(val);
    }
}

void SimulatorMainWindow::onStop()
{
    m_state = STOP;
}

void SimulatorMainWindow::onConfigure()
{
    // Unload plugin
    // auto isOk = unloadGraphPlugin();
    /*QEventLoop eventLoop;
    eventLoop.exec();
    while (!eventLoop.processEvents())
        eventLoop.quit();*/
    // Write  JSONs
    // Config:
    auto path = QString("%1/%2").arg(pluginConfigsFolder).arg("plugin_config.json");
    bool isOk = writeConfigJSON(path);

    // Graph Plots:
    writeGraphJSONs();

    // Read JSONS and load plugin again
    // loadGraphPlugin();

    // Prepare GUI
    enableConfigure(false);
}
