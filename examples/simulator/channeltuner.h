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
    MeasuredValueDescription description() const;
    //!< Amplitude + deviation * random();
    double randomValue() const;


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
