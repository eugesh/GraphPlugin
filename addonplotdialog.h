#ifndef ADDONPLOTDIALOG_H
#define ADDONPLOTDIALOG_H

#include <QDialog>

namespace Ui {
class AddOnPlotDialog;
}

class AddOnPlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddOnPlotDialog(QWidget *parent = nullptr);
    ~AddOnPlotDialog();

private:
    Ui::AddOnPlotDialog *ui;
};

#endif // ADDONPLOTDIALOG_H
