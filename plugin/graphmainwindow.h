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

/**
 * @brief The valID struct
 * Unique ID of measured value.
 */
struct valID {
    //! Number of channel (for multichannel devices).
    int chNumber;
    //! Name of physical quantity.
    QString name;
};

/**
 * @brief The GraphID struct
 * Uniqe ID of any graph figure placed on the plotting area.
 */
struct GraphID {
    //! Name of graph.
    QString graphName;
    //! Number of channel (for multichannel devices).
    int chNumber;
    //! Name of X axis
    QString xName;
    //! Name of Y axis
    QString yName;
    //! Name of Z axis
    QString zName;
};

class GraphPluginConfig;
class QCPWaterfall;

//! Necessary for GraphID comparison, e.g. in sorting operation or placing into QMap.
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
    explicit GraphMainWindow(const QString &name, const Graph::GraphProperties &properties, QWidget *parent = nullptr);
    ~GraphMainWindow();

    bool loadCSV();
    QString nameTr() const;
    void setConfig(GraphPluginConfig *config);
    bool setValuesDescriptions(const QMultiMap<QString, Graph::MeasuredValueDescription> &mvd);
    void setUpdateAble(bool isUpdateable);
    QString plotName() const { return m_plotName; }
    void setPlotName(const QString &name) { m_plotName = name; }

public slots:
    void clearAll();
    void loadCSVdialog();
    void saveCSVdialog();
    void saveJSONdialog();
    void saveImageDialog();
    void addData(const QList<MeasuredValue> &val);
    void add2dData(const QList<MeasuredValue> &val);
    QList<GraphID> addGraph(const Graph::GraphProperties &prop);
    void onRemoveJSON();

signals:
    void deleteMe();

private:
    void commonInit();
    void createCustomPlot(const QString &name);
    bool readJSON(const QString &path);
    bool saveJSON(const QString &path) const;
    static Graph::GraphProperties parseJsonObject(const QJsonObject &plotObject);
    bool removeJSON() const;
    // bool applyProperties();
    QList<GraphID> addGraph(const QString &name);
    QList<GraphID> addXYGraph(const QString &name, QCustomPlot *cplot = nullptr);
    GraphID addParametricGraph(const QString &name);
    GraphID addWaterfallGraph(const QString &name);
    GraphID addWaterfallGraph(QCustomPlot *cplot, const Graph::GraphProperties &prop);
    GraphID addHairCombGraph(const QString &name);
    GraphID addAdditionalWaterfallGraph(const QString &name);
    void removeWaterfallGraph(const QString &name);
    void saveCSV(const QString &name) const;
    void saveImage(const QString &name) const;
    void updateGraphs(const GraphID& id, double x, double y);
    void updateCurves(const GraphID& id, uint64_t ts, double x, double y);
    void updateColorMaps(const GraphID& id, uint64_t ts, QVariantList x, QVariantList y);
    void updateColorMaps(const GraphID& gid, uint64_t timestamp, const QMap<int, double> &x, const QMap<int, double> &y);
    void alignColorMaps();
    void updateAll();

private slots:
    void contextMenuRequest(const QPoint &pt);

private:
    Ui::GraphMainWindow *ui;
    QSharedPointer<QCPAxisTickerDateTime> m_ticker;
    QString m_pathToCSV;
    //!< Value descriptor, pointer to Graph
    QMultiMap<GraphID, QCPGraph*> m_valueGraphMap;
    // Parametric curves (x, y, t)
    QMap<GraphID, QCPCurve*> m_valueCurveMap;
    QMap<GraphID, QCPWaterfall*> m_valueColorMap;
    // Id of Colormap Graph -> id if Scatter Graph
    QMap<GraphID, GraphID> m_valueAdditionalGraphMap;
    QList<QCustomPlot*> m_customPlotList;
    //!< Name of value name of X as key and value name of Y as value, one-multiple
    QMultiMap<QString, QString> m_valueNameYX;
    // Graph name -> properties
    QMultiMap<QString, Graph::GraphProperties> m_properties;
    QString m_JSONPath;
    QString m_CSVPath;
    QString m_ImagePath;
    QString m_plotName;
    bool m_isLoadFromJson = false;
    // Is there any changes in JSON files
    bool m_hasUpdate = false;
    GraphPluginConfig *m_config;
    QMultiMap<QString, Graph::MeasuredValueDescription> m_measValDescMap;
    // QMap<QString, QString> m_auxPlotsMap;
    bool m_isUpdatable = true;
};

#endif // GRAPHMAINWINDOW_H
