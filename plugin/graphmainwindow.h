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
    QString zName;
};

class GraphPluginConfig;
class QCPWaterfall;

inline bool operator<(const GraphID &g1, const GraphID &g2)
{
    if (g1.chNumber != g2.chNumber)
        return g1.chNumber < g2.chNumber;
    else if (g1.yName != g2.yName)
        return g1.yName < g2.yName;
    else if (g1.xName != g2.xName)
        return g1.xName < g2.xName;
    else
        return g1.zName < g2.zName;
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
    bool setValuesDescriptions(const QMultiMap<QString, MeasuredValueDescription> &mvd);

public slots:
    void clearAll();
    void loadCSVdialog();
    void saveCSVdialog();
    void saveJSONdialog();
    void saveImageDialog();
    void addData(const QList<MeasuredValue> &val);
    void add2dData(const QList<MeasuredValue> &val);
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
    void addWaterfallGraph(const QString &name);
    void addWaterfallGraph(QCustomPlot *cplot, const GraphProperties &prop);
    void addAdditionalWaterfallGraph(const QString &name);
    void saveCSV(const QString &name) const;
    void saveImage(const QString &name) const;
    void updateGraphs(const GraphID& id, double x, double y);
    void updateCurves(const GraphID& id, uint64_t ts, double x, double y);
    void updateColorMaps(const GraphID& id, uint64_t ts, QVariantList x, QVariantList y);
    void updateColorMaps(const GraphID& gid, uint64_t timestamp, const QMap<int, double> &x, const QMap<int, double> &y);
    void alignColorMaps();

private:
    Ui::GraphMainWindow *ui;
    QSharedPointer<QCPAxisTickerDateTime> m_ticker;
    QString m_pathToCSV;
    //!< Value descriptor, pointer to Graph
    QMap<GraphID, QCPGraph*> m_valueGraphMap;
    // Parametric curves (x, y, t)
    QMap<GraphID, QCPCurve*> m_valueCurveMap;
    QMap<GraphID, QCPWaterfall*> m_valueColorMap;
    //!< Name of value name of X as key and value name of Y as value, one-multiple
    QMultiMap<QString, QString> m_valueNameYX;
    // Graph name -> properties
    QMultiMap<QString, GraphProperties> m_properties;
    QString m_JSONPath;
    QString m_CSVPath;
    QString m_ImagePath;
    QString m_plotName;
    bool m_isLoadFromJson = false;
    bool m_hasUpdate = false;
    GraphPluginConfig *m_config;
    QMultiMap<QString, MeasuredValueDescription> m_measValDescMap;
};

#endif // GRAPHMAINWINDOW_H
