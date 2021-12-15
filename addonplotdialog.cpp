#include "addonplotdialog.h"
#include "ui_addonplotdialog.h"

AddOnPlotDialog::AddOnPlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddOnPlotDialog)
{
    ui->setupUi(this);
}

AddOnPlotDialog::~AddOnPlotDialog()
{
    delete ui;
}
