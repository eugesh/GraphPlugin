#include "common.h"
#include "graphpluginconfig.h"
#include "vectorindicatorsboard.h"
#include "vectorindicatorwidget.h".h"
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
    readJSON(pathToJSON);
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
    }*/

    return restoreBoardGeometry();
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
        GraphProperties properties;
        properties.name = indicatorObject["name"].toString();
        properties.x_name = indicatorObject["x_name"].toString();
        properties.y_name = indicatorObject["y_name"].toString();
        properties.x_title = indicatorObject["x_title"].toString();
        properties.y_title = indicatorObject["y_title"].toString();
        properties.x_unit = indicatorObject["x_unit"].toString();
        properties.y_unit = indicatorObject["y_unit"].toString();
        properties.x_phisical_quantity = indicatorObject["x_phisical_quantity"].toString();
        properties.y_phisical_quantity = indicatorObject["y_phisical_quantity"].toString();
        // "channels": [1],
        // QJsonArray arr = plotObject["channels"].toArray();
        // for (auto ch : arr)
           // properties.channels << ch.toInt();

        m_properties[properties.name] = properties;
        addNewIndicator(properties);
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
        indicatorObject["x_name"] = prop.x_name;
        indicatorObject["y_name"] = prop.y_name;
        indicatorObject["x_title"] = prop.x_title;
        indicatorObject["y_title"] = prop.y_title;
        indicatorObject["x_unit"] = prop.x_unit;
        indicatorObject["y_unit"] = prop.y_unit;
        indicatorObject["x_phisical_quantity"] = prop.x_phisical_quantity;
        indicatorObject["y_phisical_quantity"] = prop.y_phisical_quantity;
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
