#ifndef DIGITALDISPLAYBOARD_H
#define DIGITALDISPLAYBOARD_H

#include "graph_interface.h"

#include <QMainWindow>
#include <QMap>

namespace Ui {
class DigitalDisplayBoard;
}

class DigitalBoardItem;
class GraphPluginConfig;
struct MeasuredValue;
struct MeasuredValueDescription;

class DigitalDisplayBoard : public QMainWindow
{
    Q_OBJECT

public:
    explicit DigitalDisplayBoard(QWidget *parent = nullptr);
    ~DigitalDisplayBoard() override;

    // Call at first!
    bool setValuesDescriptions(const QMultiMap<QString, MeasuredValueDescription> &mvd);
    void setConfig(GraphPluginConfig *config);

    // Apply the last custom settings
    bool initFromJSON(const QString &pathToJSON);

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    void addData(const QList<MeasuredValue> &vals);

private:
    bool readJSON();
    bool restoreBoardGeometry();
    bool saveBoardGeometry();

private:
    Ui::DigitalDisplayBoard *ui;

    GraphPluginConfig *m_config;
    QMultiMap<QString, MeasuredValueDescription> m_measValDescMap;
    QMap<QString, DigitalBoardItem*> m_items;
    QMap<QString, QDockWidget*> m_itemsDocks;
    QList<QString> m_activeSensorsNames;
    QString m_layoutIniFile = "layout/default.ini"; // Optional ini file to save subwindows state
};

#endif // DIGITALDISPLAYBOARD_H
