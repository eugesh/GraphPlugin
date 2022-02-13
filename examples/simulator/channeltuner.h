#ifndef CHANNELTUNER_H
#define CHANNELTUNER_H

#include "common.h"

#include <QWidget>

namespace Ui {
class ChannelTuner;
}

class ChannelConfigurationDialog;
class GraphPluginConfig;
// struct MeasuredValueDescription;

class ChannelTuner : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelTuner(int channelNumber, GraphPluginConfig *config, QWidget *parent = nullptr);
    ~ChannelTuner();

    double amplitude() const;
    double deviation() const;
    QString name() const;
    QString description() const;
    // MeasuredValueDescription description() const;
    //!< Amplitude + deviation * random();
    double randomValue() const;
    QString physicalValueName() const;
    QString measurementUnitName() const;
    QString measurementUnitNameTr() const;
    bool isUseStandardPlot() const;
    void setPhysicalValues(const QStringList &names);
    void setCurrentPhysicalValue(const QString &name);
    void setCurrentMeasurementUnit(const QString &name);
    void setName(const QString &name);
    void setDescription(const QString &name);
    void setMeasurementUnits(const QStringList &names);

signals:

private slots:
    void slotToolButton();

private:
    Ui::ChannelTuner *ui;
    MeasuredValueDescription m_prop;
    GraphPluginConfig *m_config;
    ChannelConfigurationDialog *m_dialog = nullptr;
};

#endif // CHANNELTUNER_H
