#ifndef GRAPHTABLEVIEW_H
#define GRAPHTABLEVIEW_H

#include <QObject>
#include <QTableView>


class GraphTableView final: public QTableView
{
    Q_OBJECT
public:
    explicit GraphTableView(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel *model) final;

protected:
    void keyPressEvent(QKeyEvent *event) final;

protected slots:
    void copyColumn();
    void copyRow();
    void placeOnPlot();
};

#endif // GRAPHTABLEVIEW_H
