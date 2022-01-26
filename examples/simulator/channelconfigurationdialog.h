#ifndef CHANNELCONFIGURATIONDIALOG_H
#define CHANNELCONFIGURATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ChannelConfigurationDialog;
}

class ChannelConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChannelConfigurationDialog(QWidget *parent = nullptr);
    ~ChannelConfigurationDialog();

private:
    Ui::ChannelConfigurationDialog *ui;
};

#endif // CHANNELCONFIGURATIONDIALOG_H
