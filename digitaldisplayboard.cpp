#include "common.h"
#include "digitaldisplayboard.h"
#include "digitalboarditem.h"
#include "graph_interface.h"
#include "graphpluginconfig.h"

#include "ui_digitaldisplayboard.h"

#include <QDockWidget>
#include <QStandardPaths>
#include <QSettings>

DigitalDisplayBoard::DigitalDisplayBoard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DigitalDisplayBoard)
{
    setObjectName("DigitalDisplayBoard");

    ui->setupUi(this);
}

DigitalDisplayBoard::~DigitalDisplayBoard()
{
    saveBoardGeometry(); // ToDo: is it enough or closeEvent is needed too?

    delete ui;
}

void DigitalDisplayBoard::setConfig(GraphPluginConfig *config)
{
    m_config = config;
}

bool DigitalDisplayBoard::restoreBoardGeometry()
{
    auto configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);

    //  Restore Geometry
    // QSettings settings("eugesh", "GruphPlugin");
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    auto allKeys = settings.allKeys();
    auto keys = settings.childKeys();
    auto groups = settings.childGroups();

    settings.beginGroup("DigitalDisplayBoard");

    auto geomData = settings.value("geometry").toByteArray();
    bool is_ok = QWidget::restoreGeometry(geomData);
    auto state = settings.value("windowState").toByteArray();
    is_ok = is_ok && restoreState(state);

    /*for (auto item : m_itemsDocks.values()) {
        settings.beginGroup(tr("%1").arg(item->objectName()));

        if (! settings.value("isChecked").toBool()) {
            auto act = item->toggleViewAction();
            act->setChecked(false);
            //item->hide();
        }

        settings.endGroup();
    }*/

    settings.endGroup();

    return is_ok;
}


bool DigitalDisplayBoard::saveBoardGeometry()
{
    // QSettings settings("eugesh", "DigitalDisplayBoard");
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    settings.beginGroup("DigitalDisplayBoard");

    auto geom = saveGeometry();
    settings.setValue("geometry", geom);
    auto state = saveState();
    settings.setValue("windowState", state);

    /*for (auto item : m_itemsDocks.values()) {
        settings.beginGroup(tr("%1").arg(item->objectName()));

        auto act = item->toggleViewAction();
        settings.setValue("isChecked", act->isChecked());
        //settings.setValue("isHidden", item->isHidden());

        settings.endGroup();
    }*/

    settings.endGroup();

    return true;
}

void DigitalDisplayBoard::closeEvent(QCloseEvent *event)
{
    saveBoardGeometry(); // ToDo: test on Linux or delete
    QWidget::closeEvent(event);
}

bool DigitalDisplayBoard::setValuesDescriptions(const QMap<QString, MeasuredValueDescription> &mvd)
{
    m_measValDescMap = mvd;

    m_activeSensorsNames = mvd.keys();

    return true;
}

bool DigitalDisplayBoard::initFromJSON(const QString &pathToJSON)
{
    for (auto name : m_activeSensorsNames) {
        auto desc = m_measValDescMap[name];

        auto title = desc.desc_ru;
        //m_config->auxMeasUnits(m_measValDescMap[name].physQuant)["name_ru"].toString();

        auto *item = new DigitalBoardItem(title,
                     m_config->getMultipliers(desc.physQuant), m_config->getOffsets(name), this);
        item->setWindowTitle(title);
        auto *dock = new QDockWidget(this);
        m_items.insert(name, item);
        dock->setWidget(item);
        dock->setAllowedAreas(Qt::AllDockWidgetAreas);
        dock->setObjectName(tr("%1%2").arg(name).arg("Dock"));
        m_itemsDocks.insert(name, dock);
        addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, dock);
    }

    return restoreBoardGeometry();
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
