#include "common.h"
#include "digitaldisplayboard.h"
#include "digitalboarditem.h"
#include "graphpluginconfig.h"

#include "ui_digitaldisplayboard.h"

#include <QDockWidget>

DigitalDisplayBoard::DigitalDisplayBoard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DigitalDisplayBoard)
{
    ui->setupUi(this);
}

DigitalDisplayBoard::~DigitalDisplayBoard()
{
    delete ui;
}

void DigitalDisplayBoard::setConfig(GraphPluginConfig *config)
{
    m_config = config;
}

bool DigitalDisplayBoard::setValuesDescriptions(const QMap<QString, MeasuredValueDescription> &mvd)
{
    m_measValDescMap = mvd;

    m_activeSensorsNames = mvd.keys(); // Stub

    return true;
}

bool DigitalDisplayBoard::initFromJSON(const QString &pathToJSON)
{
    for (auto name : m_activeSensorsNames) {
        auto decs = m_measValDescMap[name];
        auto *item = new DigitalBoardItem(m_config->getMultipliers(name));
        auto *dock = new QDockWidget();
        dock->setWidget(item);
        addDockWidget(Qt::DockWidgetArea::AllDockWidgetAreas, dock);
    }

    return true;
}

bool DigitalDisplayBoard::readJSON()
{

    return true;
}
