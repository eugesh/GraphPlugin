#ifndef GRAPHTABLEVIEW_H
#define GRAPHTABLEVIEW_H

#include <QObject>
#include <QTableView>

class AddOnPlotDialog;
class GraphPluginConfig;
struct GraphProperties;
struct MeasuredValueDescription;

class GraphTableView final: public QTableView
{
    Q_OBJECT
public:
    explicit GraphTableView(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel *model) final; // ToDo: place to constructor
    void setConfig(GraphPluginConfig *config); // ToDo: place to constructor
    void setMeasValues(const QMap<QString, MeasuredValueDescription> &mvd);

protected:
    void keyPressEvent(QKeyEvent *event) final;

signals:
    void createNewGraph(const QString &customPlotName, const GraphProperties &prop);

protected slots:
    void copyColumn();
    void copyRow();
    void placeOnPlot();

private:
    AddOnPlotDialog *m_addPointDialog = nullptr;
    QMap<QString, MeasuredValueDescription> m_measValuesDesc;
    // Config
    GraphPluginConfig *m_config = nullptr;
    // Header's view column index, on which context menu activated
    int m_currentColumn;
};

#endif // GRAPHTABLEVIEW_H
