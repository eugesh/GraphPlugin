#include "common.h"
#include "graphmainwindow.h"
#include "qcpwaterfall.h"
#include "ui_graphmainwindow.h"

GraphMainWindow::GraphMainWindow(const QString &path2JSON, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphMainWindow)
{
    commonInit();

    readJSON(path2JSON);
}

// Auxilliary constructor
GraphMainWindow::GraphMainWindow(const QString &name, const GraphProperties &properties, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphMainWindow)
{
    setObjectName(tr("%1%2").arg(name).arg("GraphWindow"));
    commonInit();

    createCustomPlot(name);
    m_plotName = name;

    addGraph(properties);
}

void GraphMainWindow::commonInit()
{
    ui->setupUi(this);

    m_JSONPath = "./configs/graphs";

    ui->customPlot->setContentsMargins(0, 0, 0, 0);
    //ui->customPlot->layout()->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);

    connect(ui->actionSaveJSON, &QAction::triggered, this, &GraphMainWindow::saveJSONdialog);
    connect(ui->actionSaveCSV, &QAction::triggered, this, &GraphMainWindow::saveCSVdialog);
    connect(ui->actionSaveImage, &QAction::triggered, this, &GraphMainWindow::saveImageDialog);
    connect(ui->actionRemoveJSON, &QAction::triggered, this, &GraphMainWindow::onRemoveJSON);
}

GraphMainWindow::~GraphMainWindow()
{
    if (m_hasUpdate) {
        int button = QMessageBox::question(this,
                                      tr("Внимание!"),
                                      tr("\n Есть несохраненные графики:"
                                         "\n    %1,"
                                         "\n желаете сохранить?").arg(m_plotName),
                                      QMessageBox::StandardButton::Yes,
                                      QMessageBox::StandardButton::No);
        if (QMessageBox::StandardButton::Yes == button) {
            saveJSONdialog();
        }
    }

    delete ui;
}

void GraphMainWindow::setConfig(GraphPluginConfig *config)
{
    m_config = config;
}

void GraphMainWindow::clearAll()
{
    // ui->customPlot->plotLayout()->clear();

    for (auto map : m_valueColorMap) {
        //map->data()->clear();
        // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
        /*QCPMarginGroup *marginGroup = new QCPMarginGroup(cplot);
        cplot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
        colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

        // rescale the key (x) and value (y) axes so the whole color map is visible:
        cplot->rescaleAxes();*/
        // map->data()->setSize(map->data()->keySize(), map->data()->valueSize());
        // map->rescaleDataRange();
        //delete map;
        map->data()->fill(NAN);
        map->rescaleKeyAxis();
        map->rescaleDataRange();
        ui->customPlot->rescaleAxes();
    }

    for (auto graph : m_valueGraphMap) {
        if (graph) {
            graph->clear();
            graph->rescaleAxes();
            // ui->customPlot->removeGraph(graph);
        }
    }

    ui->customPlot->replot();
    // ui->customPlot->updateLayerIndices();
    // auto keys = m_valueColorMap.keys();

    /*for (auto key : m_valueColorMap.keys()) {
        if (m_valueColorMap.value(key) != nullptr) {
            delete m_valueColorMap.value(key);
            m_valueColorMap[key] = nullptr;
            m_valueColorMap.remove(key);
        }
    }

    m_valueColorMap.clear();*/

    /*for (auto key : keys) {
        removeWaterfallGraph(key.graphName);
    }

    for (auto key : keys) {
        addGraph(key.graphName);
    }*/
}

// ToDo: change on Add. Add necessary descriptions only.
bool GraphMainWindow::setValuesDescriptions(const QMultiMap<QString, MeasuredValueDescription> &mvd)
{
    m_measValDescMap = mvd;

    return true;
}

void GraphMainWindow::createCustomPlot(const QString &name)
{
    setWindowTitle(name);

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);

    //ui->customPlot->xAxis->setRange(2e-5, 6e-5);
    //ui->customPlot->yAxis->setRange(-2, 2);
    //ui->customPlot->yAxis2->setRange(-0.025, 0.025);
    // ui->customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->customPlot->axisRect()->setupFullAxesBox();

    ui->customPlot->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(ui->customPlot, name, QFont("sans", 14, QFont::Bold));
    ui->customPlot->plotLayout()->addElement(0, 0, title);
}

QString GraphMainWindow::nameTr() const
{
    if (ui->customPlot)
        return m_plotName;
    else
        return {};
}

GraphProperties GraphMainWindow::parseJsonObject(const QJsonObject &plotObject)
{
    GraphProperties properties;
    properties.name = plotObject["name"].toString();
    properties.x_name = plotObject["x_name"].toString();
    properties.y_name = plotObject["y_name"].toString();
    properties.z_name = plotObject["z_name"].toString();
    properties.x_title = plotObject["x_title"].toString();
    properties.y_title = plotObject["y_title"].toString();
    properties.z_title = plotObject["z_title"].toString();
    properties.x_unit = plotObject["x_unit"].toString();
    properties.y_unit = plotObject["y_unit"].toString();
    properties.z_unit = plotObject["z_unit"].toString();
    properties.indexName = plotObject["index_name"].toString();
    properties.x_phisical_quantity = plotObject["x_phisical_quantity"].toString();
    properties.y_phisical_quantity = plotObject["y_phisical_quantity"].toString();
    properties.z_phisical_quantity = plotObject["z_phisical_quantity"].toString();
    properties.x_dir = static_cast<GraphDir>(plotObject["x_dir"].toInt());
    properties.y_dir = static_cast<GraphDir>(plotObject["y_dir"].toInt());
    properties.total_N = static_cast<unsigned int >(plotObject["total_N"].toInt());
    properties.last_N_limit = static_cast<unsigned int >(plotObject["last_N_limit"].toInt());
    properties.update_mode = plotObject["update_mode"].toString().contains("ALL", Qt::CaseInsensitive) ||
            plotObject["update_mode"].toString().contains("все", Qt::CaseInsensitive)? SHOW_ALL : SHOW_LAST_N;
    properties.x_scale = plotObject["x_scale"].toString().contains(tr("лог"), Qt::CaseInsensitive) ||
            plotObject["x_scale"].toString().contains(tr("log"), Qt::CaseInsensitive) ? GraphScaleType::LOG : GraphScaleType::LIN;
    properties.y_scale = plotObject["y_scale"].toString().contains(tr("лог"), Qt::CaseInsensitive) ||
                    plotObject["y_scale"].toString().contains(tr("log"), Qt::CaseInsensitive) ? GraphScaleType::LOG : GraphScaleType::LIN;
    properties.z_scale = plotObject["z_scale"].toString().contains(tr("лог"), Qt::CaseInsensitive) ||
                    plotObject["z_scale"].toString().contains(tr("log"), Qt::CaseInsensitive) ? GraphScaleType::LOG : GraphScaleType::LIN;
    properties.color = nameToColorConverter(plotObject["color"].toString());
    properties.graphType = nameToGraphTypeConverter(plotObject["graph_type"].toString());
    properties.colorScale = nameToColorScaleTypeConverter(plotObject["color_scale"].toString());
            // static_cast<GraphType> (plotObject["graph_type"].toInt());
    // properties.is_parametric = plotObject["is_parametric"].toBool();
    // properties.is_integral = plotObject["is_integral"].toBool();
    // "channels": [1],
    QJsonArray arr = plotObject["channels"].toArray();
    for (auto ch : arr)
        properties.channels << ch.toInt();

    return properties;
}

bool GraphMainWindow::readJSON(const QString &path)
{
    m_JSONPath = path;

    QFile loadFile(path);

    if (! loadFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Input file " << path << " wasn't opened on read";
        return false;
    }

    QByteArray loadData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(loadData));

    if (loadDoc.object().contains("name")) {
        m_plotName = loadDoc.object()["name"].toObject()["name"].toString();
        createCustomPlot(m_plotName);
    }

    setObjectName(tr("%1%2").arg(m_plotName).arg("GraphWindow"));
    //QString name = loadDoc.object()["name"].toObject()["name"].toString();
    //name = loadDoc["name"]["name"].toString();

    // QJsonArray plotsArray = loadDoc.object()["plots"].toArray();
    QJsonArray plotsArray = loadDoc["plots"].toArray();

    for (int i = 0; i < plotsArray.size(); ++i) {
        QJsonObject plotObject = plotsArray[i].toObject();
        auto properties = parseJsonObject(plotObject);

        m_properties.insert(properties.name, properties);
        addGraph(properties.name);

        if (plotObject.contains("additional_plots")) {
            // m_auxPlotsMap.insertMulti(properties.name, plotObject.value("additional_plots").toString());
            QJsonArray auxPlotsArray = plotObject["additional_plots"].toArray();
            for (int i = 0; i < auxPlotsArray.size(); ++i) {
                QJsonObject auxPlotObject = auxPlotsArray[i].toObject();
                auto auxPlotProperties = parseJsonObject(auxPlotObject);
                m_properties.insert(auxPlotProperties.name, auxPlotProperties);
                addGraph(auxPlotProperties.name);
            }
        }
    }

    m_isLoadFromJson = true;

    return true;
}

void GraphMainWindow::saveJSONdialog()
{
    m_JSONPath = QFileDialog::getSaveFileName(this, tr("Сохранить график как"), m_JSONPath, tr("JSON файлы(*.json)"));

    if (!m_JSONPath.endsWith(".json", Qt::CaseInsensitive))
        m_JSONPath += ".json";

    if (saveJSON(m_JSONPath))
        m_hasUpdate = false;
}

bool GraphMainWindow::saveJSON(const QString &path) const
{
    QFile saveFile(path);

    if (! saveFile.open(QIODevice::WriteOnly)) {
        qCritical() << "Output file " << path << " wasn't opened on write";
        return false;
    }

    QJsonObject docObject;
    QJsonObject docPropObject;
    QJsonArray customPlotArray;
    QJsonObject graphObject;

    docPropObject["name"] = windowTitle();
    docObject["name"] = docPropObject;

    for (auto prop : m_properties) {
        graphObject["name"] = prop.name;
        graphObject["x_name"] = prop.x_name;
        graphObject["y_name"] = prop.y_name;
        graphObject["z_name"] = prop.z_name;
        graphObject["x_title"] = prop.x_title;
        graphObject["y_title"] = prop.y_title;
        graphObject["z_title"] = prop.z_title;
        graphObject["x_unit"] = prop.x_unit;
        graphObject["y_unit"] = prop.y_unit;
        graphObject["z_unit"] = prop.z_unit;
        graphObject["index_name"] = prop.indexName;
        graphObject["x_phisical_quantity"] = prop.x_phisical_quantity;
        graphObject["y_phisical_quantity"] = prop.y_phisical_quantity;
        graphObject["x_dir"] = prop.x_dir == LEFT ? "left" : "right";
        graphObject["y_dir"] = prop.y_dir == DOWN ? "down" : "up";
        graphObject["total_N"] = static_cast<int>(prop.total_N);
        graphObject["last_N_limit"] = static_cast<int>(prop.last_N_limit);
        graphObject["update_mode"] = prop.update_mode == SHOW_LAST_N ? "SHOW_LAST_N" : "SHOW_ALL";

        graphObject["x_scale"] = prop.x_scale == GraphScaleType::LOG ? "LOG" : "LIN";
        graphObject["y_scale"] = prop.y_scale == GraphScaleType::LOG ? "LOG" : "LIN";

        graphObject["color"] = ColorToNameConverter(prop.color);

        QJsonArray channelsArray;
        for (auto ch : prop.channels) {
            channelsArray.append(ch);
        }
        graphObject["channels"] = channelsArray;
        graphObject["graph_type"] = graphTypeToNameConverter(prop.graphType);
        graphObject["color_scale"] = colorScaleTypeToNameConverter(prop.colorScale);
        // graphObject["is_parametric"] = prop.is_parametric;
        // graphObject["is_integral"] = prop.is_integral;

        customPlotArray.append(graphObject);
    }

    docObject["plots"] = customPlotArray;

    QJsonDocument saveDoc(docObject);
    saveFile.write(saveDoc.toJson());

    return true;
}

void GraphMainWindow::onRemoveJSON()
{
    auto isOk = removeJSON();

    if (isOk)
        emit deleteMe();
}

bool GraphMainWindow::removeJSON() const
{
    auto info = QFileInfo(m_JSONPath);

    QDir dir = info.absoluteDir();

    return dir.remove(info.absoluteFilePath());
}

void GraphMainWindow::saveCSVdialog()
{
    m_CSVPath = QFileDialog::getSaveFileName(this,
                                             tr("Сохранить график в CSV"),
                                             m_CSVPath,
                                             tr("(*.CSV)"));

    saveCSV(m_CSVPath);
}

void GraphMainWindow::saveCSV(const QString &name) const
{
    if (name.isEmpty())
        return;

    QFile file(name);

    if (! file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << tr("Ошибка: Файл") << name << tr(" не был открыт на запись!");
        // qCritical() << tr("File") << name << " wasn't opened on read";
        return;
    }

    QTextStream *stream = new QTextStream(&file);

    for (int i = 0; i < ui->customPlot->graphCount(); ++i) {
        for (int k = 0; ui->customPlot->graph(i)->data().data()->size(); ++k) {
            auto val = ui->customPlot->graph(i)->data().data()->at(k);
            *stream << val << ",";
        }
        *stream << '\n';
    }

    delete stream;
}

void GraphMainWindow::saveImageDialog()
{
    m_ImagePath = QFileDialog::getSaveFileName(this,
                                               tr("Сохранить график как изображение"),
                                               m_ImagePath,
                                               tr("Изображения (*.png *.bmp *.tif *.xpm *.jpg *.jpeg *.JPG)"));

    saveImage(m_ImagePath);
}

void GraphMainWindow::saveImage(const QString &name) const
{
    if (name.isEmpty())
        return;

    ui->customPlot->saveJpg(name);
}

void GraphMainWindow::addXYGraph(const QString &name)
{
    auto prop = m_properties.value(name);

    int chNum = 1;
    for (auto ch : prop.channels) {

        QCPGraph *graph = ui->customPlot->addGraph(ui->customPlot->xAxis, ui->customPlot->yAxis);
        QString graphSubTitle = name;
        if (prop.channels.count() > 1) {
            if (! graphSubTitle.isEmpty())
                graphSubTitle += tr(", ");
            graphSubTitle += tr("канал %1").arg(ch);
        }
        ui->customPlot->graph()->setName(graphSubTitle);
        ui->customPlot->graph()->setLineStyle(QCPGraph::lsLine);
        ui->customPlot->graph()->setScatterStyle(
            QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(ui->customPlot->graphCount() + 1)));

        ui->customPlot->xAxis->setLabel(prop.x_title);

        if (prop.x_phisical_quantity.contains("time", Qt::CaseInsensitive)) {
            m_ticker = QSharedPointer<QCPAxisTickerDateTime>::create();
            // m_ticker->setDateTimeFormat("hh:mm:ss\nzzz");
            m_ticker->setDateTimeFormat(QLatin1String("hh:mm:ss"));
            //mDateTimeFormat(QLatin1String("hh:mm:ss\ndd.MM.yy")),
            ui->customPlot->xAxis->setTicker(m_ticker);
            ui->customPlot->xAxis->setTickLabels(true);
            m_ticker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
        }

        ui->customPlot->yAxis->setLabel(prop.y_title);
        ui->customPlot->legend->setVisible(true);

        QPen graphPen;
        // graphPen.setColor(QColor(rand() % 245 + 10, rand() % 245 + 10, rand() % 245 + 10));
        graphPen.setColor(prop.color);
        graphPen.setWidthF(1);
        ui->customPlot->graph()->setPen(graphPen);
        ui->customPlot->replot();
        ui->customPlot->graph()->setAdaptiveSampling(true);
        ui->customPlot->graph()->setMaxCount(m_properties.value(name).total_N);
        // m_valueNameXY.insertMulti(m_properties[name].x_name, m_properties[name].y_name);
        m_valueNameYX.insertMulti(prop.y_name, prop.x_name);

        // QPair<QString, QString> xy = qMakePair<QString, QString> (m_properties[name].x_name, m_properties[name].y_name);
        // m_valueGraphMap.insert(xy, graph);

        // std::pair<QString, QString> yx = std::make_pair(prop.y_name, prop.x_name);
        GraphID gid;
        gid.graphName = name;
        gid.chNumber = ch;
        gid.xName = prop.x_name;
        gid.yName = prop.y_name;
        // QMap<graphID, QCPGraph*> m_valueGraphMap;
        m_valueGraphMap.insert(gid, graph);
        // m_valueGraphMap.insert(yx, graph);
        chNum++;
    }
}

void GraphMainWindow::addParametricGraph(const QString &name)
{
    auto prop = m_properties.value(name);

    QCPCurve *newParametricCurve = new QCPCurve(ui->customPlot->xAxis, ui->customPlot->yAxis);

    newParametricCurve->setMaxCount(m_properties.value(name).total_N);

    GraphID gid;
    gid.graphName = name;
    gid.chNumber = 1;
    gid.xName = prop.x_name;
    gid.yName = prop.y_name;

    ui->customPlot->xAxis->setLabel(prop.x_title);
    ui->customPlot->yAxis->setLabel(prop.y_title);
    ui->customPlot->legend->setVisible(true);

    QPen graphPen;
    graphPen.setColor(prop.color);
    graphPen.setWidthF(1);
    //ui->customPlot->graph()->setPen(graphPen);
    ui->customPlot->replot();
    // m_valueNameXY.insertMulti(m_properties[name].x_name, m_properties[name].y_name);
    m_valueNameYX.insert(prop.y_name, prop.x_name);

    m_valueCurveMap.insert(gid, newParametricCurve);
    newParametricCurve->setScatterStyle(
        QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(ui->customPlot->plottableCount() + 1)));
}

void GraphMainWindow::removeWaterfallGraph(const QString &name)
{
    GraphID graphId;
    for (auto gid : m_valueColorMap.keys()) {
        if (gid.graphName == name)
            graphId = gid;
    }


    if (m_valueColorMap.value(graphId))
        delete m_valueColorMap.value(graphId);
    m_valueNameYX.remove(graphId.yName, graphId.zName);
}

void GraphMainWindow::addWaterfallGraph(const QString &name)
{
    auto prop = m_properties.value(name);

    if (! m_valueNameYX.isEmpty()) {
        addAdditionalWaterfallGraph(name);
        return;
    } else {
        addWaterfallGraph(ui->customPlot, prop);
    }
}

void GraphMainWindow::addWaterfallGraph(QCustomPlot *cplot, const GraphProperties &prop)
{
    cplot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    cplot->axisRect()->setupFullAxesBox(true);
    cplot->xAxis->setLabel(prop.x_title);
    cplot->yAxis->setLabel(prop.y_title);
    cplot->yAxis->setRangeReversed(true);

    QCPWaterfall *colorMap = new QCPWaterfall(cplot->xAxis, cplot->yAxis);
    int nx = 256;
    int ny = 16;
    colorMap->data()->setSize(nx, ny); // we want the color map to have nx * ny data points

    if (prop.x_phisical_quantity.contains("time", Qt::CaseInsensitive)) {
        m_ticker = QSharedPointer<QCPAxisTickerDateTime>::create();
        m_ticker->setDateTimeFormat(QLatin1String("hh:mm:ss"));
        cplot->xAxis->setTicker(m_ticker);
        cplot->xAxis->setTickLabels(true);
        m_ticker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    }

    // add a color scale:
    QCPWaterfallScale *colorScale = new QCPWaterfallScale(cplot);
    int row = cplot->plotLayout()->rowCount() - 1;
    cplot->plotLayout()->addElement(row, 1, colorScale); // add it to the right of the main axis rect
    colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    colorMap->setColorScale(colorScale); // associate the color map with the color scale
    colorScale->axis()->setLabel(prop.z_title);
    colorScale->setGradient(prop.colorScale);

    for (int xIndex = 0; xIndex < nx; ++xIndex) {
        for (int yIndex = 0; yIndex < ny; ++yIndex) {
            // Set background as invalid values
            colorMap->data()->setCell(xIndex, yIndex, NAN);
        }
    }

    // set the color gradient of the color map to one of the presets:
    // colorMap->setGradient(prop.colorScale);//QCPColorGradient::gpPolar);
    // we could have also created a QCPColorGradient instance and added own colors to
    // the gradient, see the documentation of QCPColorGradient for what's possible.

    // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
    colorMap->rescaleDataRange();

    // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    QCPMarginGroup *marginGroup = new QCPMarginGroup(cplot);
    cplot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    // rescale the key (x) and value (y) axes so the whole color map is visible:
    cplot->rescaleAxes();
    cplot->replot();

    GraphID gid;
    gid.graphName = prop.name;
    gid.chNumber = 1;
    gid.xName = prop.x_name;
    gid.yName = prop.y_name;
    gid.zName = prop.z_name;

    m_valueColorMap.insert(gid, colorMap);
    m_valueNameYX.insert(prop.y_name, prop.z_name);
}

void GraphMainWindow::addAdditionalWaterfallGraph(const QString &name)
{
    auto prop = m_properties.value(name);

    QCustomPlot *customPlot2 = new QCustomPlot(ui->centralwidget);
    ui->gridLayout->addWidget(customPlot2, 1, 0, 1, 1);

    addWaterfallGraph(customPlot2, prop);
}

void GraphMainWindow::addGraph(const QString &name)
{
    if (!m_properties.contains(name))
        return;

    auto prop = m_properties.value(name);

    switch (prop.graphType) {
    case GraphScatter:
        addXYGraph(name);
        break;
    case GraphParametric:
        addParametricGraph(name);
        break;
    case GraphIntegral:
        addParametricGraph(name);
        break;
    case GraphColorMap:
        addWaterfallGraph(name);
        break;
    }

    // Additional plots
    if (m_auxPlotsMap.contains(name)) {
        addXYGraph(name);
    }

    /* if (prop.is_parametric) {
        addParametricGraph(name);
    } else {
        addXYGraph(name);
    }*/
}

void GraphMainWindow::addGraph(const GraphProperties &prop)
{
    m_properties.insertMulti(prop.name, prop);

    m_hasUpdate = true;

    addGraph(prop.name);
}

/*bool GraphMainWindow::applyProperties()
{

    return true;
}*/

bool GraphMainWindow::loadCSV()
{

    return true;
}

void GraphMainWindow::loadCSVdialog()
{

}

void GraphMainWindow::updateGraphs(const GraphID& gid, double x, double y)
{
    QCPGraph *graph = nullptr;

    graph = m_valueGraphMap[gid];

    if (graph) {
        auto name = graph->name();
        graph->addData(x, y);
        if (m_properties.value(name).update_mode == SHOW_ALL)
            graph->rescaleAxes();
    }
}

void GraphMainWindow::updateCurves(const GraphID& gid, uint64_t timestamp, double x, double y)
{
    QCPCurve *curve = nullptr;

    curve = m_valueCurveMap[gid];

    QString name;
    if (gid.graphName.isEmpty()) {
        name = m_valueCurveMap.find(gid).key().graphName;
    }

    if (curve) {
        if (m_properties.value(name).graphType == GraphIntegral) {
            if (curve->dataCount() >= 1) {
                double tsPrev = curve->data()->at(curve->dataCount() - 1)->t;
                double dt = (timestamp - tsPrev) / 1000; // Msec to sec
                double newX = curve->data()->at(curve->dataCount() - 1)->key + x * dt;
                double newY = curve->data()->at(curve->dataCount() - 1)->value + y * dt;
                curve->addData(timestamp, newX, newY);
            } else {
                curve->addData(timestamp, 0, 0);
            }
        } else {
            curve->addData(timestamp, x, y);
        }
        curve->rescaleAxes();
    }
}

void GraphMainWindow::updateColorMaps(const GraphID& gid, uint64_t timestamp, QVariantList x, QVariantList y)
{
    QCPWaterfall *colorMap = nullptr;

    colorMap = m_valueColorMap[gid];

    QString name;
    if (gid.graphName.isEmpty()) {
        name = m_valueColorMap.find(gid).key().graphName;
    }

    if (!colorMap)
        return;

    // Depth
    QList<double> depths;
    for (auto val : x)
        depths << val.toDouble();

    // Magnitudes or Angles
    QList<double> magnitudes;
    for (auto val : y)
        magnitudes << val.toDouble();

    // Refresh size of Color Map if changed
    if (Q_UNLIKELY(magnitudes.count() != colorMap->data()->valueSize()))
        colorMap->data()->setSize(colorMap->data()->keySize(), qMax(magnitudes.count(), colorMap->data()->valueSize()));

    // Add data
    if (m_properties.value(name).graphType == GraphColorMap)
        colorMap->addData(timestamp, depths, magnitudes);

    // Replot
    static_cast<QCustomPlot*>(colorMap->parent())->replot();

    // Align if there are 2 Color Maps
    if (m_valueColorMap.count() > 1)
        alignColorMaps();
}

void GraphMainWindow::updateColorMaps(const GraphID& gid, uint64_t timestamp, const QMap<int, double> &x, const QMap<int, double> &y)
{
    QCPWaterfall *colorMap = nullptr;

    colorMap = m_valueColorMap[gid];

    if (colorMap->data()->valueSize() < x.size())
        colorMap->data()->setValueSize(x.size());

    QString name;
    if (gid.graphName.isEmpty())
        name = m_valueColorMap.find(gid).key().graphName;

    if (!colorMap)
        return;

    // Depth
    QVector<double> depths(colorMap->data()->valueSize(), NAN);
    for (auto xKey : x.keys())
        depths[xKey] = x.value(xKey);

    // Magnitudes or Angles
    QVector<double> magnitudes(colorMap->data()->valueSize(), NAN);
    for (auto yKey : y.keys())
        magnitudes[yKey] = y.value(yKey);

    // Refresh size of Color Map if changed
    if (Q_UNLIKELY(magnitudes.count() != colorMap->data()->valueSize()))
        colorMap->data()->setSize(colorMap->data()->keySize(), qMax(magnitudes.count(), colorMap->data()->valueSize()));

    // Add data
    if (m_properties.value(name).graphType == GraphColorMap)
        colorMap->addData(timestamp, depths.toList(), magnitudes.toList());

    // Replot
    static_cast<QCustomPlot*>(colorMap->parent())->replot();

    // Align if there are 2 Color Maps
    if (m_valueColorMap.count() > 1)
        alignColorMaps();
}

void GraphMainWindow::alignColorMaps()
{
    auto firstMap = m_valueColorMap.first();
    auto secondMap = m_valueColorMap.last();

    auto firstW = firstMap->colorScale()->axis()->margin();
    auto secondW = secondMap->colorScale()->axis()->margin();

    if (firstW > secondW) {
        auto dx = firstW - secondW;
        firstMap->colorScale()->setMargins(QMargins(0, 0, 0, 0));
        secondMap->colorScale()->setMargins(QMargins(0, 0, dx, 0));
    }

    if (firstW < secondW) {
        auto dx = secondW - firstW;
        secondMap->colorScale()->setMargins(QMargins(0, 0, 0, 0));
        firstMap->colorScale()->setMargins(QMargins(0, 0, dx, 0));
    }
}

void GraphMainWindow::addData(const QList<MeasuredValue> &packet)
{
    for (MeasuredValue val1 : packet) {
        if (!val1.is_valid)
            continue;
        // auto val1_desc = m_measValDescMap.value(val1.name);
        for (QString val2_name : m_valueNameYX.values(val1.name)) {
            // If X is time
            if (val2_name.contains("time")) {
                GraphID gid;
                gid.xName = tr("time");
                gid.yName = val1.name;
                gid.chNumber = val1.channel;
                ui->customPlot->xAxis->setTickLabels(true);
                double val = val1.value.type() == QVariant::List ?
                    val1.value.toList().first().toDouble() : val1.value.toDouble();
                updateGraphs(gid, val1.timestamp, val);
            } else {
                for (MeasuredValue val2 : packet) {
                    if (!val2.is_valid)
                        continue;
                    // auto val2_desc = m_measValDescMap.value(val2.name);
                    if (val2_name == val2.name) {
                        GraphID gid;
                        gid.xName = val2.name;
                        gid.yName = val1.name;
                        gid.chNumber = val1.channel;
                        updateGraphs(gid, val2.value.toDouble(), val1.value.toDouble());
                        auto value2 = val2.value.toDouble();
                        if (val2.value.type() == QVariant::List)
                            value2 = val2.value.toList().first().toDouble();
                        auto value1 = val1.value.toDouble();
                        if (val1.value.type() == QVariant::List)
                            value1 = val1.value.toList().first().toDouble();
                        updateCurves(gid, val2.timestamp, value2, value1);
                    }
                }
            }
        }
    }

    // Update graphs
    ui->customPlot->replot();
}

void GraphMainWindow::add2dData(const QList<MeasuredValue> &packet)
{
    bool hasColormap = false;
    for (auto prop : m_properties) {
        if (prop.graphType == GraphColorMap)
            hasColormap = true;
    }

    if (!hasColormap)
        // For Color Maps only
        return;

    for (MeasuredValue val1 : packet) {
        if (!val1.is_valid)
            continue;
        for (QString val2_name : m_valueNameYX.values(val1.name)) {
            // If X is time
            if (val2_name.contains("time")) {
                GraphID gid;
                gid.xName = tr("time");
                gid.yName = val1.name;
                gid.chNumber = val1.channel;
                ui->customPlot->xAxis->setTickLabels(true);
            } else {
                for (MeasuredValue val2 : packet) {
                    if (!val2.is_valid)
                        continue;
                    if (val2_name == val2.name) {
                        if (val1.value.type() == QVariant::List) {
                            GraphID gid_;
                            gid_.xName = "time";
                            gid_.yName = val1.name;
                            gid_.zName = val2.name;
                            gid_.chNumber = 1;
                            QString name;
                            if (gid_.graphName.isEmpty()) {
                                name = m_valueColorMap.find(gid_).key().graphName;
                            }
                            auto indexName = m_properties.value(name).indexName;
                            QMap<int, double> yMap, zMap;
                            for (MeasuredValue val_index : packet) {
                                if (val_index.name.toUpper() == indexName.toUpper()) {
                                    for (int i = 0; i < val_index.value.toList().size(); ++i) {
                                        int index = val_index.value.toList()[i].toInt() - 1;
                                        double y = val1.value.toList()[i].toDouble();
                                        double z = val2.value.toList()[i].toDouble();
                                        yMap.insert(index, y);
                                        zMap.insert(index, z);
                                    }
                                }
                            }
                            updateColorMaps(gid_, val2.timestamp, yMap, zMap);
                        }
                    }
                }
            }
        }
    }

    // Update graphs
    ui->customPlot->replot();
}
