#include "common.h"
#include "graphpluginconfig.h"
#include "vectorindicatorsboard.h"
#include "vectorindicatorwidget.h".h"
#include "ui_vectorindicatorsboard.h"

#include <QDockWidget>
#include <QSettings>
#include <cmath>

VectorIndicatorsBoard::VectorIndicatorsBoard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VectorIndicatorsBoard)
{
    setObjectName("VectorIndicatorsBoard");

    ui->setupUi(this);

    ui->toolBar->toggleViewAction()->setEnabled(false);
    ui->toolBar->toggleViewAction()->setVisible(false);
    removeAction(ui->toolBar->toggleViewAction());
}

VectorIndicatorsBoard::~VectorIndicatorsBoard()
{
    saveBoardGeometry(); // ToDo: is it enough or closeEvent is needed too?

    delete ui;
}

// Call at first!
bool VectorIndicatorsBoard::setValuesDescriptions(const QMap<QString, MeasuredValueDescription> &mvd)
{
    m_measValDescMap = mvd;

    m_activeSensorsNames = mvd.keys();

    return true;
}

void VectorIndicatorsBoard::setConfig(GraphPluginConfig *config)
{
    m_config = config;
}

// Apply the last custom settings
bool VectorIndicatorsBoard::initFromJSON(const QString &pathToJSON)
{
    /*for (auto name : m_activeSensorsNames) {
        auto desc = m_measValDescMap[name];

        auto title = desc.desc_ru;

        auto *item = new VectorIndicatorWidget(title, this);
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

    return restoreBoardGeometry();*/
}

void VectorIndicatorsBoard::addNewIndicator(const GraphProperties &prop)
{
    auto *item = new VectorIndicatorWidget(prop.name, this);
    item->setWindowTitle(prop.name);

    auto *dock = new QDockWidget(this);
    m_items.insert(prop.name, item);
    dock->setWidget(item);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setObjectName(tr("%1%2").arg(prop.name).arg("Dock"));
    dock->toggleViewAction()->setText(prop.name); //desc.desc_ru);
    m_itemsDocks.insert(prop.name, dock);
    addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, dock);

    m_xyMap.insert(prop.x_name, prop.y_name);
    m_namePropMap.insert(prop.name, prop);
    QPair<QString, QString> xy = qMakePair<QString, QString> (prop.x_name, prop.y_name);
    m_xyNameMap.insert(xy, prop.name);
}

void VectorIndicatorsBoard::addData(const QList<MeasuredValue> &vals)
{
    for (auto xval : vals) {
        if (m_xyMap.contains(xval.name)) {
            auto xValName = xval.name;
            auto yVAlName = m_xyMap[xval.name];
            auto widgetName = m_xyNameMap[qMakePair<QString, QString> (xValName, yVAlName)];
            for (auto yval : vals) {
                if (yval.name == yVAlName) {
                    auto angle = atan2(yval.value, xval.value);
                    auto mag = sqrt(yval.value * yval.value + xval.value * xval.value);
                    m_items[widgetName]->setAngle(angle * 180 / M_PI);
                    m_items[widgetName]->setMagnitude(mag);
                }
            }
        }
    }
}

bool VectorIndicatorsBoard::readJSON()
{

}

bool VectorIndicatorsBoard::restoreBoardGeometry()
{
    // Restore Geometry
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    auto allKeys = settings.allKeys();
    auto keys = settings.childKeys();
    auto groups = settings.childGroups();

    settings.beginGroup(objectName());

    auto geomData = settings.value("geometry").toByteArray();
    bool is_ok = QWidget::restoreGeometry(geomData);
    auto state = settings.value("windowState").toByteArray();
    is_ok = is_ok && restoreState(state);

    settings.endGroup();

    return is_ok;
}

bool VectorIndicatorsBoard::saveBoardGeometry()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    settings.beginGroup(objectName());

    auto geom = saveGeometry();
    settings.setValue("geometry", geom);
    auto state = saveState();
    settings.setValue("windowState", state);

    settings.endGroup();

    return true;
}

void VectorIndicatorsBoard::closeEvent(QCloseEvent *event)
{
    saveBoardGeometry(); // ToDo: test on Linux or delete
    QWidget::closeEvent(event);
}
