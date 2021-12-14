#ifndef GRAPHMAINWINDOW_H
#define GRAPHMAINWINDOW_H

#include "graph_interface.h"
#include "qcustomplot.h"
#include <QMainWindow>
#include <QPair>

namespace Ui {
class GraphMainWindow;
}

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

private:
    Ui::GraphMainWindow *ui;
    QString m_pathToCSV;
    //!< Value name, pointer to Graph
    QMap<QPair<QString, QString>, QCPGraph*> m_valueGraphMap;
    QMap<QString, QPair<QString, QString> > m_graphXYnamesMap;
    //!< Name of value name of X as key and value name of Y as value, one-multiple
    QMap<QString, QString> m_valueNameXY;
};

#endif // GRAPHMAINWINDOW_H
