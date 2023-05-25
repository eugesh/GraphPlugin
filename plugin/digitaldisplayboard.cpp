#include "digitaldisplayboard.h"
#include "digitalboarditem.h"
#include "graph_interface.h"
#include "graphpluginconfig.h"

#include "ui_digitaldisplayboard.h"

#include <QDebug>
#include <QDockWidget>
#include <QStandardPaths>
#include <QSettings>

using namespace Graph;

/**
 * @brief DigitalDisplayBoard::DigitalDisplayBoard
 * Board with mainwindow to store dockable windows with LCD indicators.
 * @param parent parent widget, optional.
 */
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

// Reads QSettings to restore states and geometry of dockable subwindows.
bool DigitalDisplayBoard::restoreBoardGeometry()
{
    // auto configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);

    //  Restore Geometry
    QDir lay_dir(m_layoutIniFile);
    QScopedPointer<QSettings> settings;
    auto pwd = QDir::currentPath();

    if (lay_dir.exists(pwd + "/" + m_layoutIniFile)) {
        settings.reset(new QSettings(pwd + "/" + m_layoutIniFile, QSettings::IniFormat));
    } else {
        settings.reset(new QSettings(QApplication::organizationName(), QApplication::applicationName()));
    }
    if (settings.isNull())
        return false;

    //  Restore Geometry
    auto allKeys = settings->allKeys();
    auto keys = settings->childKeys();
    auto groups = settings->childGroups();

    settings->beginGroup("DigitalDisplayBoard");

    auto geomData = settings->value("geometry").toByteArray();
    bool is_ok = QWidget::restoreGeometry(geomData);
    if (!is_ok)
        qWarning() << "Error: failed to restore DigitalDisplayBoard geometry";
    auto state = settings->value("windowState").toByteArray();
    bool is_ok2 = restoreState(state);
    if (!is_ok2)
        qWarning() << "Error: failed to restore DigitalDisplayBoard state";
    /*for (auto item : m_itemsDocks.values()) {
        settings.beginGroup(tr("%1").arg(item->objectName()));


        settings.endGroup();
    }*/

    settings->endGroup();

    return is_ok;
}

// Saves states and geometry of dockable subwindows to QSettings.
bool DigitalDisplayBoard::saveBoardGeometry()
{
    QDir lay_dir(m_layoutIniFile);
    QScopedPointer<QSettings> settings;
    auto pwd = QDir::currentPath();

    if (lay_dir.exists(pwd + "/" + m_layoutIniFile)) {
        settings.reset(new QSettings(pwd + "/" + m_layoutIniFile, QSettings::IniFormat));
    } else {
        settings.reset(new QSettings(pwd + "/" + QApplication::organizationName(), QApplication::applicationName()));
    }
    if (settings.isNull())
        return false;

    settings->beginGroup("DigitalDisplayBoard");

    auto geom = saveGeometry();
    settings->setValue("geometry", geom);
    auto state = saveState();
    settings->setValue("windowState", state);

    /*for (auto item : m_itemsDocks.values()) {
        settings.beginGroup(tr("%1").arg(item->objectName()));
        settings.endGroup();
    }*/

    settings->endGroup();

    return true;
}

void DigitalDisplayBoard::closeEvent(QCloseEvent *event)
{
    saveBoardGeometry(); // ToDo: test on Linux or delete
    QWidget::closeEvent(event);
}

/**
 * @brief DigitalDisplayBoard::setValuesDescriptions
 * @param mvd map "Measured Value name" -> "struct MeasuredValueDescription".
 * @return true|false.
 */
bool DigitalDisplayBoard::setValuesDescriptions(const QMultiMap<QString, Graph::MeasuredValueDescription> &mvd)
{
    m_measValDescMap = mvd;

    m_activeSensorsNames = mvd.keys();

    return true;
}

/**
 * @brief DigitalDisplayBoard::initFromJSON
 * @param pathToJSON
 * @return
 */
bool DigitalDisplayBoard::initFromJSON(const QString &pathToJSON)
{
    for (auto name : m_activeSensorsNames) {
        auto desc = m_measValDescMap.value(name);

        auto title = desc.desc_ru;

        auto *item = new DigitalBoardItem(title,
                                          desc.symbol_rus,
                                          m_config->getMultipliersWithPrefixes(desc.physQuant),
                                          m_config->getOffsets(name),
                                          this);
        item->setWindowTitle(title);
        auto *dock = new QDockWidget(this);
        m_items.insert(name, item);
        dock->setWidget(item);
        dock->setAllowedAreas(Qt::AllDockWidgetAreas);
        dock->setObjectName(tr("%1%2").arg(name).arg("Dock"));
        dock->toggleViewAction()->setText(desc.desc_ru);
        m_itemsDocks.insert(name, dock);
        addDockWidget(Qt::TopDockWidgetArea, dock);
    }

    return restoreBoardGeometry();
}

bool DigitalDisplayBoard::readJSON()
{

    return true;
}

void DigitalDisplayBoard::addData(const QList<MeasuredValue> &vals)
{
    for (const auto &val : vals) {
        if (!m_items.contains(val.name))
            continue;
        if (val.value.type() == QVariant::List) {
            if (val.value.toList().size() == 1 && val.is_valid) {
                auto t = val.value.toList().first().toDouble();
                m_items.value(val.name)->setCurrentValue(t);
                m_items.value(val.name)->setEnabled(true);
            } else {
                continue;
                m_items.value(val.name)->setEnabled(false);
            }
        } else if (/*!m_items.contains(val.name) ||*/ val.value.type() == QVariant::Map) {
            continue;
        } else if (val.is_valid) {
            m_items.value(val.name)->setCurrentValue(val.value.toDouble());
            m_items.value(val.name)->setEnabled(true);
        } else {
            m_items.value(val.name)->setEnabled(false);
        }
    }
}
