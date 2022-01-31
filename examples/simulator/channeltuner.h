#ifndef CHANNELTUNER_H
#define CHANNELTUNER_H

#include "common.h"

#include <QWidget>

namespace Ui {
class ChannelTuner;
}

// struct MeasuredValueDescription;

class ChannelTuner : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelTuner(int channelNumber, QWidget *parent = nullptr);
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
};

#endif // CHANNELTUNER_H
