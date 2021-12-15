#include "common.h"
#include "graphmainwindow.h"
#include "ui_graphmainwindow.h"

/*enum GraphDir {
    LEFT,
    UP,
    RIGHT,
    DOWN
};

enum GraphUpdateMode {
    SHOW_ALL,
    SHOW_LAST_N,
    DONT_UPDATE
};

enum GraphScaleType {
    LIN,
    LOG
};

struct GraphProperties {
    QString name;
    QString x_name;
    QString y_name;
    QString x_unit;
    QString y_unit;
    QString x_phisical_quantity;
    QString y_phisical_quantity;
    GraphDir x_dir;
    GraphDir y_dir;
    unsigned int total_N;
    unsigned int last_N_limit;
    GraphUpdateMode update_mode;
    GraphScaleType x_scale;
    GraphScaleType y_scale;
    Qt::GlobalColor color;
};*/

GraphMainWindow::GraphMainWindow(const QString &path2JSON, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphMainWindow)
{
    ui->setupUi(this);

    readJSON(path2JSON);
}

//GraphMainWindow::GraphMainWindow(const QString &name, const QList<GraphProperties> &properties, QWidget *parent) :
GraphMainWindow::GraphMainWindow(const QString &name, const GraphProperties &properties, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphMainWindow)
{
    ui->setupUi(this);

    createCustomPlot(name);

    addGraph(properties);
}

GraphMainWindow::~GraphMainWindow()
{
    delete ui;
}

void GraphMainWindow::createCustomPlot(const QString &name)
{
    setObjectName(name);
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

bool GraphMainWindow::readJSON(const QString &path)
{
    QFile loadFile(path);

    if (! loadFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Input file " << path << " wasn't opened on read";
        return false;
    }

    QByteArray loadData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(loadData));

    QString name;
    if (loadDoc.object().contains("name")) {
        name = loadDoc.object()["name"].toObject()["name"].toString();
        createCustomPlot(name);
    }
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
        properties.x_unit = plotObject["x_unit"].toString();
        properties.y_unit = plotObject["y_unit"].toString();
        properties.x_phisical_quantity = plotObject["x_phisical_quantity"].toString();
        properties.y_phisical_quantity = plotObject["y_phisical_quantity"].toString();
        properties.x_dir = static_cast<GraphDir>(plotObject["x_dir"].toInt());
        properties.y_dir = static_cast<GraphDir>(plotObject["y_dir"].toInt());
        properties.total_N = plotObject["total_N"].toInt();
        properties.last_N_limit = plotObject["last_N_limit"].toInt();
        properties.update_mode = static_cast<GraphUpdateMode>(plotObject["update_mode"].toInt());
        properties.x_scale = static_cast<GraphScaleType>(plotObject["x_scale"].toInt());
        properties.y_scale = static_cast<GraphScaleType>(plotObject["y_scale"].toInt());
        properties.color = nameToColorConverter(plotObject["color"].toString());
        m_properties[properties.name] = properties;
        addGraph(properties.name);
    }

    return true;
}

bool GraphMainWindow::saveJSON(const QString &path)
{

    return true;
}

void GraphMainWindow::addGraph(const QString &name)
{
    if (! m_properties.contains(name))
        return;

    QCPGraph *graph = ui->customPlot->addGraph(ui->customPlot->xAxis, ui->customPlot->yAxis);
    ui->customPlot->graph()->setName(name);
    ui->customPlot->graph()->setLineStyle(QCPGraph::lsLine);

    QPen graphPen;
    // graphPen.setColor(QColor(rand() % 245 + 10, rand() % 245 + 10, rand() % 245 + 10));
    graphPen.setColor(m_properties[name].color);
    graphPen.setWidthF(1);
    ui->customPlot->graph()->setPen(graphPen);
    ui->customPlot->replot();
    // m_valueNameXY.insertMulti(m_properties[name].x_name, m_properties[name].y_name);
    m_valueNameYX.insertMulti(m_properties[name].y_name, m_properties[name].x_name);

    // QPair<QString, QString> xy = qMakePair<QString, QString> (m_properties[name].x_name, m_properties[name].y_name);
    // m_valueGraphMap.insert(xy, graph);

    QPair<QString, QString> yx = qMakePair<QString, QString> (m_properties[name].y_name, m_properties[name].x_name);
    m_valueGraphMap.insert(yx, graph);
}

void GraphMainWindow::addGraph(const GraphProperties &prop)
{
    m_properties.insert(prop.name, prop);

    addGraph(prop.name);
}

bool GraphMainWindow::applyProperties()
{

    return true;
}

bool GraphMainWindow::loadCSV()
{

    return true;
}

void GraphMainWindow::loadCSVdialog()
{

}

void GraphMainWindow::saveCSVdialog()
{

}

void GraphMainWindow::addData(const QList<MeasuredValue> &packet)
{
    QCPGraph *graph;

    for (MeasuredValue val1 : packet)
        for (QString val2_name : m_valueNameYX.values(val1.name)) {
            if (val2_name.contains("time")) { // If X is time
                graph = m_valueGraphMap[qMakePair(val1.name, tr("time"))];
                if (graph) {
                    graph->addData(val1.timestamp, val1.value);
                    graph->rescaleAxes();
                }
            } else {
                for (MeasuredValue val2 : packet)
                    if (val2_name == val2.name) {
                        graph = m_valueGraphMap[qMakePair(val1.name, val2.name)];
                        graph->addData(val1.value, val2.value);
                        graph->rescaleAxes();
                    }
            }
        }

    // Update graphs
    ui->customPlot->replot();
}
