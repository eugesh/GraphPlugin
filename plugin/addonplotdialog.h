#ifndef ADDONPLOTDIALOG_H
#define ADDONPLOTDIALOG_H

#include <QDialog>
#include <QMap>

#include "common.h"

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
    void setGraphProperties(const Graph::GraphProperties &defaultProp);
    void setMeasValDesc(const QMultiMap<QString, Graph::MeasuredValueDescription> &mvd);

    QString getCustomPlotName() const;
    Graph::GraphProperties getProp() const;
    void setProp(const Graph::GraphProperties &prop);
    void setTitle(const QString& name);

    QVector<int> channels() const;

private:
    Ui::AddOnPlotDialog *ui;
    GraphPluginConfig *m_config;
    QMultiMap<QString, Graph::MeasuredValueDescription> m_measValuesDesc;
};

#endif // ADDONPLOTDIALOG_H
