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
    void setPhysicalValues(const QStringList &names);
    void setCurrentPhysicalValue(const QString &name);
    void setCurrentMeasurementUnit(const QString &name);
    void setName(const QString &name);
    void setDescription(const QString &name);
    // void setMeasurementUnits(const QStringList &names);

public slots:
    void onComboBoxPhyscsChanged(const QString &name);

private:
    void fillForm();

private:
    Ui::ChannelConfigurationDialog *ui;
    GraphPluginConfig *m_config;
};

#endif // CHANNELCONFIGURATIONDIALOG_H
