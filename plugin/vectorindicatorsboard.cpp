#include "graphpluginconfig.h"
#include "vectorindicatorsboard.h"
#include "vectorindicatorwidget.h"
#include "ui_vectorindicatorsboard.h"

#include <QDebug>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <cmath>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

using namespace Graph;

/**
 * @brief VectorIndicatorsBoard::VectorIndicatorsBoard
 * Implements Gauge Indicators board.
 * @param parent
 */
VectorIndicatorsBoard::VectorIndicatorsBoard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VectorIndicatorsBoard)
{
    setObjectName("VectorIndicatorsBoard");

    ui->setupUi(this);

    // Disallow user to hide toolbar
    ui->toolBar->toggleViewAction()->setEnabled(false);
    ui->toolBar->toggleViewAction()->setVisible(false);
    removeAction(ui->toolBar->toggleViewAction());

    connect(ui->actionSaveJson, &QAction::triggered, this, &VectorIndicatorsBoard::onSaveJsonAs);
    connect(ui->actionRemoveJson, &QAction::triggered, this, &VectorIndicatorsBoard::onRemoveJson);
}

VectorIndicatorsBoard::~VectorIndicatorsBoard()
{
    saveBoardGeometry(); // ToDo: is it enough or closeEvent is needed too?

    delete ui;
}

// Call at first!
/**
 * @brief VectorIndicatorsBoard::setValuesDescriptions
 * @param mvd MultiMap: Measured Value Name -> MeasuredValueDescription structure.
 * @return true|false.
 */
bool VectorIndicatorsBoard::setValuesDescriptions(const QMultiMap<QString, Graph::MeasuredValueDescription> &mvd)
{
    m_measValDescMap = mvd;

    m_activeSensorsNames = mvd.keys();

    return true;
}

/**
 * @brief VectorIndicatorsBoard::setConfig
 * Set pointer to \link GraphPluginConfig \endlink
 * @param config
 */
void VectorIndicatorsBoard::setConfig(GraphPluginConfig *config)
{
    m_config = config;
}

/**
 * @brief VectorIndicatorsBoard::initFromJSON
 * Initialization.
 * @param pathToJSON path to JSON file.
 * @return
 */
bool VectorIndicatorsBoard::initFromJSON(const QString &pathToJSON)
{
    bool is_ok = readJSON(pathToJSON);

    return is_ok && restoreBoardGeometry();
}

/**
 * @brief VectorIndicatorsBoard::addNewIndicator
 * Add new arrow-dial indicator.
 * @param prop indicator properties. The same as for graphs.
 */
void VectorIndicatorsBoard::addNewIndicator(const Graph::GraphProperties &prop)
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
    Graph::GraphProperties prop_tmp = prop;
    std::pair<QString, QString> xy = std::make_pair(prop_tmp.x_name, prop_tmp.y_name);
    m_xyNameMap.insert(xy, prop.name);
}

/**
 * @brief VectorIndicatorsBoard::addData
 * Update indicator with current data.
 * @param vals list of \link MeasuredValue \endlink.
 */
void VectorIndicatorsBoard::addData(const QList<MeasuredValue> &vals)
{
    for (auto xval : vals) {
        if (!xval.is_valid)
            continue;
        if (m_xyMap.contains(xval.name)) {
            auto xValName = xval.name;
            auto yVAlName = m_xyMap[xval.name];
            auto widgetName = m_xyNameMap[std::make_pair(xValName, yVAlName)];
            for (auto yval : vals) {
                if (!yval.is_valid)
                    continue;
                if (yval.name == yVAlName) {
                    double mag, angle;
                    if (m_properties.value(widgetName).graphType == GraphPolar) {
                        angle = xval.value.toDouble();
                        mag = yval.value.toDouble();
                    } else {
                        auto X = xval.value.toDouble();
                        auto Y = yval.value.toDouble();
                        angle = atan2(Y, X);
                        mag = sqrt(Y * Y + X * X);
                    }
                    m_items[widgetName]->setAngle(angle);// * 180 / M_PI);
                    m_items[widgetName]->setMagnitude(mag);
                    m_items[widgetName]->setEnabled(true);
                }
            }
        }
    }
}

void VectorIndicatorsBoard::onSaveJsonAs()
{
    m_JSONPath = QFileDialog::getSaveFileName(this, tr("Сохранить конфигурацию панели стрелочных индикаторов в JSON"),
                                              m_JSONPath, tr("JSON файлы (*.json)"));

    if (!m_JSONPath.endsWith(".json", Qt::CaseInsensitive))
        m_JSONPath += ".json";

    if (saveJSON(m_JSONPath))
        m_hasUpdate = false;
}

void VectorIndicatorsBoard::onRemoveJson()
{
    auto info = QFileInfo(m_JSONPath);

    QDir dir = info.absoluteDir();

    auto isOk = dir.remove(info.absoluteFilePath());

    if (isOk)
        emit deleteMe();
}

bool VectorIndicatorsBoard::readJSON(const QString &path)
{
    m_JSONPath = path;

    QFile loadFile(path);

    if (! loadFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Input file " << path << " wasn't opened on read";
        return false;
    }

    QByteArray loadData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(loadData));

    QJsonArray widgetsArray = loadDoc["indicators"].toArray();

    for (int i = 0; i < widgetsArray.size(); ++i) {
        QJsonObject indicatorObject = widgetsArray[i].toObject();
        Graph::GraphProperties properties;
        properties.name = indicatorObject["name"].toString();
        properties.x_name = indicatorObject["phi_name"].toString();
        properties.y_name = indicatorObject["mag_name"].toString();
        properties.x_title = indicatorObject["phi_title"].toString();
        properties.y_title = indicatorObject["mag_title"].toString();
        properties.x_unit = indicatorObject["phi_unit"].toString();
        properties.y_unit = indicatorObject["mag_unit"].toString();
        properties.x_phisical_quantity = indicatorObject["phi_phisical_quantity"].toString();
        properties.y_phisical_quantity = indicatorObject["mag_phisical_quantity"].toString();
        properties.graphType = nameToGraphTypeConverter(indicatorObject["graph_type"].toString());
        // "channels": [1],
        // QJsonArray arr = plotObject["channels"].toArray();
        // for (auto ch : arr)
           // properties.channels << ch.toInt();

        m_properties[properties.name] = properties;
        addNewIndicator(properties);
        m_items[properties.name]->setAutoDisableOnIdle(indicatorObject["disable_timeout_ms"].toInt());
    }

    // m_isLoadFromJson = true;

    return true;
}

bool VectorIndicatorsBoard::saveJSON(const QString &path) const
{
    QFile saveFile(path);

    if (! saveFile.open(QIODevice::WriteOnly)) {
        qCritical() << "Output file " << path << " wasn't opened on write";
        return false;
    }

    QJsonObject docObject;
    //QJsonObject docPropObject;
    QJsonArray widgetsPlotArray;
    QJsonObject indicatorObject;

    // docPropObject["name"] = windowTitle();
    // docObject["name"] = docPropObject;

    for (auto prop : m_properties) {
        indicatorObject["name"] = prop.name;
        indicatorObject["phi_name"] = prop.x_name;
        indicatorObject["mag_name"] = prop.y_name;
        indicatorObject["phi_title"] = prop.x_title;
        indicatorObject["mag_title"] = prop.y_title;
        indicatorObject["phi_unit"] = prop.x_unit;
        indicatorObject["mag_unit"] = prop.y_unit;
        indicatorObject["phi_phisical_quantity"] = prop.x_phisical_quantity;
        indicatorObject["mag_phisical_quantity"] = prop.y_phisical_quantity;
        //graphObject["color"] = ColorToNameConverter(prop.color);

        /*QJsonArray channelsArray;
        for (auto ch : prop.channels) {
            channelsArray.append(ch);
        }*/
        //graphObject["channels"] = channelsArray;
        widgetsPlotArray.append(indicatorObject);
    }

    docObject["indicators"] = widgetsPlotArray;

    QJsonDocument saveDoc(docObject);
    saveFile.write(saveDoc.toJson());

    return true;
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

void VectorIndicatorsBoard::setAutoDisableOnIdle(int timeout)
{
    m_autoHideTimeout = timeout;

    for (auto item : m_items.values()) {
        item->setAutoDisableOnIdle(timeout);
    }
}
