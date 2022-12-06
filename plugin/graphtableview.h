#ifndef GRAPHTABLEVIEW_H
#define GRAPHTABLEVIEW_H

#include "common.h"

#include <QObject>
#include <QTableView>

class AddOnPlotDialog;
class GraphPluginConfig;
struct GraphProperties;

class GraphTableView final: public QTableView
{
    Q_OBJECT
public:
    explicit GraphTableView(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel *model) final; // ToDo: place to constructor
    void setConfig(GraphPluginConfig *config); // ToDo: place to constructor
    void setMeasValues(const QMultiMap<QString, MeasuredValueDescription> &mvd);

protected:
    void keyPressEvent(QKeyEvent *event) final;

signals:
    void createNewGraph(const QString &customPlotName, const GraphProperties &prop);
    void createNewVectorIndicator(const QString &customPlotName, const GraphProperties &prop);

protected slots:
    void copyColumn();
    void copyRow();
    void placeOnPlot();
    void placeOnVectorIndicator();

private:
    GraphProperties graphPropertiesFromSelectedColoumn(const QString &name) const;

private:
    AddOnPlotDialog *m_addPlotDialog = nullptr;
    QMultiMap<QString, MeasuredValueDescription> m_measValuesDesc;
    // Config
    GraphPluginConfig *m_config = nullptr;
    // Header's view column index, on which context menu activated
    int m_currentColumn;
    QMenu *m_contextMenu = nullptr;
    QAction *m_action1 = nullptr, *m_action2 = nullptr;
};

#endif // GRAPHTABLEVIEW_H
