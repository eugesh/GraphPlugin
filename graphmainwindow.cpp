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

QColor nameToColorConverter(const QString &name) {
    if (name.contains("black", Qt::CaseInsensitive))
        return Qt::black;
    else if (name.contains("white", Qt::CaseInsensitive))
        return Qt::white;
    else if (name.contains("lght", Qt::CaseInsensitive) &&
             name.contains("gray", Qt::CaseInsensitive))
        return Qt::lightGray;
    else if (name.contains("dark", Qt::CaseInsensitive) &&
             name.contains("gray", Qt::CaseInsensitive))
        return Qt::darkGray;
    else if (name.contains("dark", Qt::CaseInsensitive) &&
             name.contains("red", Qt::CaseInsensitive))
        return Qt::darkRed;
    else if (name.contains("dark", Qt::CaseInsensitive) &&
             name.contains("blue", Qt::CaseInsensitive))
        return Qt::darkBlue;
    else if (name.contains("dark", Qt::CaseInsensitive) &&
             name.contains("green", Qt::CaseInsensitive))
        return Qt::darkGreen;
    else if (name.contains("dark", Qt::CaseInsensitive) &&
             name.contains("Cyan", Qt::CaseInsensitive))
        return Qt::darkCyan;
    else if (name.contains("dark", Qt::CaseInsensitive) &&
             name.contains("Magenta", Qt::CaseInsensitive))
        return Qt::darkMagenta;
    else if (name.contains("dark", Qt::CaseInsensitive) &&
             name.contains("yellow", Qt::CaseInsensitive))
        return Qt::darkYellow;

    else if (name.contains("red", Qt::CaseInsensitive))
        return Qt::red;
    else if (name.contains("blue", Qt::CaseInsensitive))
        return Qt::blue;
    else if (name.contains("green", Qt::CaseInsensitive))
        return Qt::green;
    else if (name.contains("Cyan", Qt::CaseInsensitive))
        return Qt::cyan;
    else if (name.contains("Magenta", Qt::CaseInsensitive))
        return Qt::magenta;
    else if (name.contains("yellow", Qt::CaseInsensitive))
        return Qt::yellow;

    return QColor(rand() % 245 + 10, rand() % 245 + 10, rand() % 245 + 10);
}

GraphMainWindow::GraphMainWindow(const QString &path2JSON, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphMainWindow)
{
    ui->setupUi(this);

    readJSON(path2JSON);
}

GraphMainWindow::~GraphMainWindow()
{
    delete ui;
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
        setObjectName(name);
        setWindowTitle(name);

        ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                      QCP::iSelectLegend | QCP::iSelectPlottables);
        ui->customPlot->axisRect()->setupFullAxesBox();

        ui->customPlot->plotLayout()->insertRow(0);
        QCPTextElement *title = new QCPTextElement(ui->customPlot, name, QFont("sans", 14, QFont::Bold));
        ui->customPlot->plotLayout()->addElement(0, 0, title);
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

void GraphMainWindow::addGraph(const QString &name)
{
    if (! m_properties.contains(name))
        return;

    ui->customPlot->addGraph(ui->customPlot->xAxis, ui->customPlot->yAxis);
    ui->customPlot->graph()->setName(name);
    ui->customPlot->graph()->setLineStyle(QCPGraph::lsLine);

    QPen graphPen;
    // graphPen.setColor(QColor(rand() % 245 + 10, rand() % 245 + 10, rand() % 245 + 10));
    graphPen.setColor(m_properties[name].color);
    graphPen.setWidthF(1);
    ui->customPlot->graph()->setPen(graphPen);
    ui->customPlot->replot();
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
    for (MeasuredValue val1 : packet)
        for (QString val2_name : m_valueNameXY[val1.name])
            for (MeasuredValue val2 : packet)
                if (val2_name == val2.name)
                    m_valueGraphMap[qMakePair(val1.name, val2.name)]->addData(val1.value, val2.value);
}
