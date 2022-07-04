#include "common.h"
#include "digitaldisplayboard.h"
#include "digitalboarditem.h"
#include "graph_interface.h"
#include "graphpluginconfig.h"

#include "ui_digitaldisplayboard.h"

#include <QDebug>
#include <QDockWidget>
#include <QStandardPaths>
#include <QSettings>

DigitalDisplayBoard::DigitalDisplayBoard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DigitalDisplayBoard)
{
    setObjectName("DigitalDisplayBoard");

    ui->setupUi(this);

    ui->DigitalScreenToolBar->toggleViewAction()->setEnabled(false);
    ui->DigitalScreenToolBar->toggleViewAction()->setVisible(false);
    removeAction(ui->DigitalScreenToolBar->toggleViewAction());
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


        settings.endGroup();
    }*/

    settings.endGroup();

    return is_ok;
}


bool DigitalDisplayBoard::saveBoardGeometry()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    settings.beginGroup("DigitalDisplayBoard");

    auto geom = saveGeometry();
    settings.setValue("geometry", geom);
    auto state = saveState();
    settings.setValue("windowState", state);

    /*for (auto item : m_itemsDocks.values()) {
        settings.beginGroup(tr("%1").arg(item->objectName()));
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

        auto *item = new DigitalBoardItem(title, desc.symbol_rus,
                     m_config->getMultipliersWithPrefixes(desc.physQuant), m_config->getOffsets(name), this);
        item->setWindowTitle(title);
        auto *dock = new QDockWidget(this);
        m_items.insert(name, item);
        dock->setWidget(item);
        dock->setAllowedAreas(Qt::AllDockWidgetAreas);
        dock->setObjectName(tr("%1%2").arg(name).arg("Dock"));
        dock->toggleViewAction()->setText(desc.desc_ru);
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
        if (m_items.contains(val.name) && val.is_valid)
            m_items[val.name]->setCurrentValue(val.value);
        else
            m_items[val.name]->setEnabled(false);
    }
}
