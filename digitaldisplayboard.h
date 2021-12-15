#ifndef DIGITALDISPLAYBOARD_H
#define DIGITALDISPLAYBOARD_H

#include <QMainWindow>
#include <QMap>

namespace Ui {
class DigitalDisplayBoard;
}

class DigitalBoardItem;
class GraphPluginConfig;
struct MeasuredValueDescription;

class DigitalDisplayBoard : public QMainWindow
{
    Q_OBJECT

public:
    explicit DigitalDisplayBoard(QWidget *parent = nullptr);
    ~DigitalDisplayBoard();

    // Call at first!
    bool setValuesDescriptions(const QMap<QString, MeasuredValueDescription> &mvd);
    void setConfig(GraphPluginConfig *config);

    // Apply the last custom settings
    bool initFromJSON(const QString &pathToJSON);

private:
    bool readJSON();

private:
    Ui::DigitalDisplayBoard *ui;

    GraphPluginConfig *m_config;
    QMap<QString, MeasuredValueDescription> m_measValDescMap;
    QMap<QString, DigitalBoardItem> m_items;
    QList<QString> m_activeSensorsNames;
};

#endif // DIGITALDISPLAYBOARD_H
