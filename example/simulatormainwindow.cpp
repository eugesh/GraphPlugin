#include "channeltuner.h"
#include "graph_interface.h"
#include "simulatormainwindow.h"
#include "ui_simulatormainwindow.h"

#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QFormLayout>
#include <QPluginLoader>

SimulatorMainWindow::SimulatorMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButtonAddChannel, &QPushButton::pressed, this, &SimulatorMainWindow::addChannel);
    connect(ui->pushButtonRemoveChannel, &QPushButton::pressed, this, &SimulatorMainWindow::removeChannel);

    m_formLayout = new QFormLayout(this);
    ui->widget->setLayout(m_formLayout);

    loadGraphPlugin();
}

SimulatorMainWindow::~SimulatorMainWindow()
{
    graphInterface->saveGraphPluginGeometry();

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
            }); // graphInterface, &GraphInterface::addData

            if (graphInterface) {
                return true;
            }

            qWarning() << "Graph Plugin wasn't loaded!";
            pluginLoader.unload();
        }
    }

    return false;
}

void SimulatorMainWindow::addChannel()
{
    m_channelNum++;

    ChannelTuner *ct = new ChannelTuner(m_channelNum);

    m_formLayout->addRow(ct);
}

void SimulatorMainWindow::removeChannel()
{
    int lastRow = m_formLayout->count() - 1;

    if (!lastRow)
        return;

    m_channelNum--;

    m_formLayout->removeRow(lastRow);
}

void SimulatorMainWindow::onRun()
{

}

void SimulatorMainWindow::onStop()
{

}
