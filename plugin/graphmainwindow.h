#ifndef GRAPHMAINWINDOW_H
#define GRAPHMAINWINDOW_H

#include "common.h"
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
struct GraphID {
    QString graphName;
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
    bool is_parametric = false;
    bool is_integral = false;
};

class GraphPluginConfig;

inline bool operator<(const GraphID &g1, const GraphID &g2)
{
    if (g1.chNumber != g2.chNumber)
        return g1.chNumber < g2.chNumber;
    else if (g1.yName != g2.yName)
        return g1.yName < g2.yName;
    else
        return g1.xName < g2.xName;
}

class GraphMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphMainWindow(const QString &pathToJSON, QWidget *parent = nullptr);
    explicit GraphMainWindow(const QString &name, const GraphProperties &properties, QWidget *parent = nullptr);
    ~GraphMainWindow();

    bool loadCSV();
    QString nameTr() const;
    void setConfig(GraphPluginConfig *config);
    bool setValuesDescriptions(const QMap<QString, MeasuredValueDescription> &mvd);

public slots:
    void loadCSVdialog();
    void saveCSVdialog();
    void saveJSONdialog();
    void saveImageDialog();
    void addData(const QList<MeasuredValue> &val);
    void addGraph(const GraphProperties &prop);
    void onRemoveJSON();

signals:
    void deleteMe();

private:
    void commonInit();
    void createCustomPlot(const QString &name);
    bool readJSON(const QString &path);
    bool saveJSON(const QString &path) const;
    bool removeJSON() const;
    // bool applyProperties();
    void addGraph(const QString &name);
    void addXYGraph(const QString &name);
    void addParametricGraph(const QString &name);
    void saveCSV(const QString &name) const;
    void saveImage(const QString &name) const;
    void updateGraphs(const GraphID& id, double x, double y);
    void updateCurves(const GraphID& id, uint64_t ts, double x, double y);

private:
    Ui::GraphMainWindow *ui;
    QSharedPointer<QCPAxisTickerDateTime> m_ticker;
    QString m_pathToCSV;
    //!< Value name, pointer to Graph
    // QMap<QPair<QString, QString>, QCPGraph*> m_valueGraphMap;
    QMap<GraphID, QCPGraph*> m_valueGraphMap;
    // Parametric curves (x, y, t)
    QMap<GraphID, QCPCurve*> m_valueCurveMap;
    // QMap<QString, QPair<QString, QString> > m_graphXYnamesMap;
    //!< Name of value name of X as key and value name of Y as value, one-multiple
    //QMap<QString, QString> m_valueNameXY;
    QMap<QString, QString> m_valueNameYX;
    // Graph name -> properties
    QMap<QString, GraphProperties> m_properties;
    QString m_JSONPath;
    QString m_CSVPath;
    QString m_ImagePath;
    QString m_plotName;
    bool m_isLoadFromJson = false;
    bool m_hasUpdate = false;
    GraphPluginConfig *m_config;
    QMap<QString, MeasuredValueDescription> m_measValDescMap;
};

#endif // GRAPHMAINWINDOW_H
