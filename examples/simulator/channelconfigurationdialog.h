#ifndef CHANNELCONFIGURATIONDIALOG_H
#define CHANNELCONFIGURATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ChannelConfigurationDialog;
}

class GraphPluginConfig;

class ChannelConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChannelConfigurationDialog(GraphPluginConfig *config, QWidget *parent = nullptr);
    ~ChannelConfigurationDialog();

    void fillForm();

public slots:
    void onComboBoxPhyscsChanged(const QString &name);

private:
    Ui::ChannelConfigurationDialog *ui;
    GraphPluginConfig *m_config;
};

#endif // CHANNELCONFIGURATIONDIALOG_H
