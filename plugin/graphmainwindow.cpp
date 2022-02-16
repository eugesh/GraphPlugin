#include "common.h"
#include "graphmainwindow.h"
#include "ui_graphmainwindow.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <utility>
#endif


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

    connect(ui->actionSaveJSON, &QAction::triggered, this, &GraphMainWindow::saveJSONdialog);
    connect(ui->actionSaveCSV, &QAction::triggered, this, &GraphMainWindow::saveCSVdialog);
    connect(ui->actionSaveImage, &QAction::triggered, this, &GraphMainWindow::saveImageDialog);
    connect(ui->actionRemoveJSON, &QAction::triggered, this, &GraphMainWindow::onRemoveJSON);
}

GraphMainWindow::~GraphMainWindow()
{
    if (! m_isLoadFromJson || m_hasUpdate) {
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
        GraphProperties properties;
        properties.name = plotObject["name"].toString();
        properties.x_name = plotObject["x_name"].toString();
        properties.y_name = plotObject["y_name"].toString();
        properties.x_title = plotObject["x_title"].toString();
        properties.y_title = plotObject["y_title"].toString();
        properties.x_unit = plotObject["x_unit"].toString();
        properties.y_unit = plotObject["y_unit"].toString();
        properties.x_phisical_quantity = plotObject["x_phisical_quantity"].toString();
        properties.y_phisical_quantity = plotObject["y_phisical_quantity"].toString();
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
        properties.color = nameToColorConverter(plotObject["color"].toString());
        // "channels": [1],
        QJsonArray arr = plotObject["channels"].toArray();
        for (auto ch : arr)
            properties.channels << ch.toInt();

        m_properties[properties.name] = properties;
        addGraph(properties.name);
    }

    m_isLoadFromJson = true;

    return true;
}

void GraphMainWindow::saveJSONdialog()
{
    m_JSONPath = QFileDialog::getSaveFileName(this, tr("Сохранить график как"), m_JSONPath, tr("(*.JSON)"));

    // saveJSON(m_JSONPath);
    saveJSON(m_JSONPath, m_properties.values(), windowTitle());
}

/*bool GraphMainWindow::saveJSON(const QString &path) const
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
        graphObject["x_title"] = prop.x_title;
        graphObject["y_title"] = prop.y_title;
        graphObject["x_unit"] = prop.x_unit;
        graphObject["y_unit"] = prop.y_unit;
        graphObject["x_phisical_quantity"] = prop.x_phisical_quantity;
        graphObject["y_phisical_quantity"] = prop.y_phisical_quantity;
        graphObject["x_dir"] = prop.x_dir == LEFT ? "left" : "right";
        graphObject["y_dir"] = prop.y_dir == DOWN ? "down" : "up";
        graphObject["total_N"] = static_cast<int>(prop.total_N);
        graphObject["last_N_limit"] = static_cast<int>(prop.last_N_limit);
        graphObject["update_mode"] = prop.update_mode == SHOW_LAST_N ? "SHOW_LAST_N" : "SHOW_ALL";

        graphObject["x_scale"] = prop.x_scale == GraphScaleType::LOG ? "LOG" : "LIN";
        graphObject["y_scale"] = prop.y_scale == GraphScaleType::LOG ? "LOG" : "LIN";

        graphObject["y_unit"] = ColorToNameConverter(prop.color);

        QJsonArray channelsArray;
        for (auto ch : prop.channels) {
            channelsArray.append(ch);
        }
        graphObject["channels"] = channelsArray;

        customPlotArray.append(graphObject);
    }

    docObject["plots"] = customPlotArray;

    QJsonDocument saveDoc(docObject);
    saveFile.write(saveDoc.toJson());

    return true;
}*/

void GraphMainWindow::onRemoveJSON()
{
    auto isOk = removeJSON();

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

void GraphMainWindow::addGraph(const QString &name)
{
    if (! m_properties.contains(name))
        return;

    auto prop = m_properties[name];

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
        ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(chNum)));

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
        // m_valueNameXY.insertMulti(m_properties[name].x_name, m_properties[name].y_name);
        m_valueNameYX.insert(prop.y_name, prop.x_name);

        // QPair<QString, QString> xy = qMakePair<QString, QString> (m_properties[name].x_name, m_properties[name].y_name);
        // m_valueGraphMap.insert(xy, graph);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QPair<QString, QString> yx = std::make_pair(prop.y_name, prop.x_name);
#else
        QPair<QString, QString> yx = qMakePair<QString, QString> (prop.y_name, prop.x_name);
#endif
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

void GraphMainWindow::addGraph(const GraphProperties &prop)
{
    // m_properties.insertMulti(prop.name, prop);
    m_properties.insert(prop.name, prop);

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

void GraphMainWindow::addData(const QList<MeasuredValue> &packet)
{
    QCPGraph *graph;

    for (MeasuredValue val1 : packet) {
        for (QString val2_name : m_valueNameYX.values(val1.name)) {
            // If X is time
            if (val2_name.contains("time")) {
                GraphID gid;
                gid.xName = tr("time");
                gid.yName = val1.name;
                gid.chNumber = val1.channel;
                // graph = m_valueGraphMap[qMakePair(val1.name, tr("time"))];
                graph = m_valueGraphMap[gid];
                ui->customPlot->xAxis->setTickLabels(true);
                if (graph) {
                    //auto date = QDateTime::fromMSecsSinceEpoch(val1.timestamp);
                    graph->addData(val1.timestamp, val1.value);
                    graph->rescaleAxes();
                }
            } else {
                for (MeasuredValue val2 : packet) {
                    if (val2_name == val2.name) {
                        GraphID gid;
                        gid.xName = val2.name;
                        gid.yName = val1.name;
                        gid.chNumber = val1.channel;
                        // graph = m_valueGraphMap[qMakePair(val1.name, val2.name)];
                        graph = m_valueGraphMap[gid];
                        if (graph) {
                            graph->addData(val2.value, val1.value);
                            graph->rescaleAxes();
                        }
                    }
                }
            }
        }
    }

    // Update graphs
    ui->customPlot->replot();
}
