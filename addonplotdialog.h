#ifndef ADDONPLOTDIALOG_H
#define ADDONPLOTDIALOG_H

#include <QDialog>

namespace Ui {
class AddOnPlotDialog;
}

struct GraphProperties;

class AddOnPlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddOnPlotDialog(QWidget *parent = nullptr);
    ~AddOnPlotDialog();

    void setConfig(const GraphProperties &defaultProp);
    GraphProperties getProp() const;

private:
    Ui::AddOnPlotDialog *ui;
};

#endif // ADDONPLOTDIALOG_H
