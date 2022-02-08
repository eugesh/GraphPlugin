#include "channelconfigurationdialog.h"
#include "channeltuner.h"
#include "graph_interface.h"
#include "graphpluginconfig.h"
#include "simulatormainwindow.h"
#include "ui_simulatormainwindow.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
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

    m_config = new GraphPluginConfig(siPath, prefPath);

    srand(time(0));
}

SimulatorMainWindow::~SimulatorMainWindow()
{
    if (graphInterface) {
        graphInterface->saveGraphPluginGeometry();

    }

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
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            graphInterface = qobject_cast<GraphInterface *>(plugin);

            graphInterface->setMainWindow(this);

            graphInterface->loadJSONs();

            connect(this, &SimulatorMainWindow::newData, [&](const MeasuredValue &val) {
                graphInterface->addData(val);
            });

            if (graphInterface) {
                return true;
            }

            qWarning() << "Graph Plugin wasn't loaded!";
            pluginLoader.unload();
        }
    }

    return false;
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

    ChannelTuner *ct = new ChannelTuner(m_channelNum, m_config);

    m_formLayout->addRow(ct);
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
    if (m_enableConfigure) {
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

    // Write  JSONs

    // Read JSONS and load plugin again

    // Prepare GUI
    enableConfigure(false);
}
