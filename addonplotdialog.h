#ifndef ADDONPLOTDIALOG_H
#define ADDONPLOTDIALOG_H

#include <QDialog>

namespace Ui {
class AddOnPlotDialog;
}

// Config
class GraphPluginConfig;
struct GraphProperties;

class AddOnPlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddOnPlotDialog(QWidget *parent = nullptr);
    ~AddOnPlotDialog();

    void setConfig(GraphPluginConfig *config);
    void setGraphProperties(const GraphProperties &defaultProp);

    GraphProperties getProp() const;

private:
    Ui::AddOnPlotDialog *ui;
    GraphPluginConfig *m_config;
};

#endif // ADDONPLOTDIALOG_H
