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

    //!< Whether add standard time plot - f(t)
    bool addTimePlotChecked() const;
    QString measurementValueName() const;
    QString description() const;
    QString physicalValueName() const;
    QString measurementUnitName() const;
    QString measurementUnitNameTr() const;

public slots:
    void onComboBoxPhyscsChanged(const QString &name);

private:
    void fillForm();

private:
    Ui::ChannelConfigurationDialog *ui;
    GraphPluginConfig *m_config;
};

#endif // CHANNELCONFIGURATIONDIALOG_H
