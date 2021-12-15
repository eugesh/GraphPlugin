#ifndef GRAPHTABLEVIEW_H
#define GRAPHTABLEVIEW_H

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

protected:
    void keyPressEvent(QKeyEvent *event) final;

signals:
    void createNewGraph(const GraphProperties &prop);

protected slots:
    void copyColumn();
    void copyRow();
    void placeOnPlot();

private:
    AddOnPlotDialog *m_addPointDialog;
    // Config
    GraphPluginConfig *m_config;
};

#endif // GRAPHTABLEVIEW_H
