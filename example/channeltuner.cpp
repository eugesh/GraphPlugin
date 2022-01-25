#include "channeltuner.h"
#include "ui_channeltuner.h"

ChannelTuner::ChannelTuner(int channelNumber, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelTuner)
{
    ui->setupUi(this);

    ui->label->setText("Channel " + QString::number(channelNumber));
}

ChannelTuner::~ChannelTuner()
{
    delete ui;
}
