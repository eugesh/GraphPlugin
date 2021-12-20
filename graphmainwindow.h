#ifndef GRAPHMAINWINDOW_H
#define GRAPHMAINWINDOW_H

#include "graph_interface.h"
#include "qcustomplot.h"
#include <QMainWindow>
#include <QPair>

namespace Ui {
class GraphMainWindow;
}

enum GraphDir {
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

// Unique ID of measured value
struct valID {
    // Number of channel
    int chNumber;
    // Name of physical quantity
    QString name;
};

// Unique ID of graph figure
struct graphID {
    // Number of channel
    int chNumber;
    // Names of axes
    QString xName;
    QString yName;
};

struct GraphProperties {
    QString name;
    QString x_name;
    QString y_name;
    QString x_title;
    QString y_title;
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
    QVector<int> channels; // e.g. 1, 2, 3, 4
    QColor color;
};

class GraphMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphMainWindow(const QString &pathToJSON, QWidget *parent = nullptr);
    explicit GraphMainWindow(const QString &name, const GraphProperties &properties, QWidget *parent = nullptr);
    ~GraphMainWindow();

    bool loadCSV();

public slots:
    void loadCSVdialog();
    void saveCSVdialog();
    void saveJSONdialog();
    void addData(const QList<MeasuredValue> &val);
    void addGraph(const GraphProperties &prop);

private:
    void commonInit();
    void createCustomPlot(const QString &name);
    bool readJSON(const QString &path);
    bool saveJSON(const QString &path);
    bool applyProperties();
    void addGraph(const QString &name);

private:
    Ui::GraphMainWindow *ui;
    QString m_pathToCSV;
    //!< Value name, pointer to Graph
    // QMap<QPair<QString, QString>, QCPGraph*> m_valueGraphMap;
    QMap<graphID, QCPGraph*> m_valueGraphMap;
    // QMap<QString, QPair<QString, QString> > m_graphXYnamesMap;
    //!< Name of value name of X as key and value name of Y as value, one-multiple
    //QMap<QString, QString> m_valueNameXY;
    QMap<QString, QString> m_valueNameYX;
    QMap<QString, GraphProperties> m_properties;
    QString m_JSONPath;
};

#endif // GRAPHMAINWINDOW_H
