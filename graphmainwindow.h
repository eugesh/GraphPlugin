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
    QColor color;
};

class GraphMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphMainWindow(const QString &pathToJSON, QWidget *parent = nullptr);
    ~GraphMainWindow();

    bool loadCSV();

public slots:
    void loadCSVdialog();
    void saveCSVdialog();
    void addData(const QList<MeasuredValue> &val);

private:
    bool readJSON(const QString &path);
    void addGraph(const QString &name);

private:
    Ui::GraphMainWindow *ui;
    QString m_pathToCSV;
    //!< Value name, pointer to Graph
    QMap<QPair<QString, QString>, QCPGraph*> m_valueGraphMap;
    QMap<QString, QPair<QString, QString> > m_graphXYnamesMap;
    //!< Name of value name of X as key and value name of Y as value, one-multiple
    QMap<QString, QString> m_valueNameXY;
    QMap<QString, GraphProperties> m_properties;
};

#endif // GRAPHMAINWINDOW_H
