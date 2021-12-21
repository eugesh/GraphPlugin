#include "common.h"
#include "digitaldisplayboard.h"
#include "digitalboarditem.h"
#include "graph_interface.h"
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

        auto title = name; //m_config->auxMeasUnits(m_measValDescMap[name].physQuant)["name_ru"].toString();

        auto *item = new DigitalBoardItem(title, m_config->getMultipliers(name), m_config->getOffsets(name), this);
        item->setWindowTitle(title);
        auto *dock = new QDockWidget(this);
        m_items.insert(name, item);
        dock->setWidget(item);
        dock->setAllowedAreas(Qt::AllDockWidgetAreas);
        addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, dock);
    }

    return true;
}

bool DigitalDisplayBoard::readJSON()
{

    return true;
}

void DigitalDisplayBoard::addData(const QList<MeasuredValue> &vals)
{
    for (auto val : vals) {
        m_items[val.name]->setCurrentValue(val.value);
    }
}
