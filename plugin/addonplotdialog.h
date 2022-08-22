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
    explicit AddOnPlotDialog(QWidget *parent = nullptr, int channelsNum = 1);
    ~AddOnPlotDialog();

    void setConfig(GraphPluginConfig *config);
    void setGraphProperties(const GraphProperties &defaultProp);
    void setMeasValDesc(const QMultiMap<QString, MeasuredValueDescription> &mvd);

    QString getCustomPlotName() const;
    GraphProperties getProp() const;
    void setProp(const GraphProperties &prop);
    void setTitle(const QString& name);

    QVector<int> channels() const;

private:
    Ui::AddOnPlotDialog *ui;
    GraphPluginConfig *m_config;
    QMultiMap<QString, MeasuredValueDescription> m_measValuesDesc;
};

#endif // ADDONPLOTDIALOG_H
