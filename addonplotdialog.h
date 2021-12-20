#ifndef ADDONPLOTDIALOG_H
#define ADDONPLOTDIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui {
class AddOnPlotDialog;
}

// Config
class GraphPluginConfig;
struct GraphProperties;
struct MeasuredValueDescription;

class AddOnPlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddOnPlotDialog(int channelsNum = 1, QWidget *parent = nullptr);
    ~AddOnPlotDialog();

    void setConfig(GraphPluginConfig *config);
    void setGraphProperties(const GraphProperties &defaultProp);
    void setMeasValDesc(const QMap<QString, MeasuredValueDescription> &mvd);

    QString getCustomPlotName() const;
    GraphProperties getProp() const;
    void setProp(const GraphProperties &prop);

    QVector<int> channels() const;

private:
    Ui::AddOnPlotDialog *ui;
    GraphPluginConfig *m_config;
    QMap<QString, MeasuredValueDescription> m_measValuesDesc;
};

#endif // ADDONPLOTDIALOG_H
