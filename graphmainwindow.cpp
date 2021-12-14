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
        properties.color = static_cast<Qt::GlobalColor>(plotObject["color"].toInt());
        m_properties[properties.name] = properties;
    }

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
    for (MeasuredValue val1 : packet)
        for (QString val2_name : m_valueNameXY[val1.name])
            for (MeasuredValue val2 : packet)
                if (val2_name == val2.name)
                    m_valueGraphMap[qMakePair(val1.name, val2.name)]->addData(val1.value, val2.value);
}
