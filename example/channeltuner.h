#ifndef CHANNELTUNER_H
#define CHANNELTUNER_H

#include <QWidget>

namespace Ui {
class ChannelTuner;
}

class ChannelTuner : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelTuner(int channelNumber, QWidget *parent = nullptr);
    ~ChannelTuner();

private:
    Ui::ChannelTuner *ui;
};

#endif // CHANNELTUNER_H
