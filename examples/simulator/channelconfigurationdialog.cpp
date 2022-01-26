#include "channelconfigurationdialog.h"
#include "ui_channelconfigurationdialog.h"


ChannelConfigurationDialog::ChannelConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChannelConfigurationDialog)
{
    ui->setupUi(this);
}

ChannelConfigurationDialog::~ChannelConfigurationDialog()
{
    delete ui;
}
