#include "channelconfigurationdialog.h"
#include "channeltuner.h"
#include "ui_channeltuner.h"

#include <cmath>


ChannelTuner::ChannelTuner(int channelNumber, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelTuner)
{
    ui->setupUi(this);

    ui->label->setText("Channel " + QString::number(channelNumber));

    connect(ui->toolButton, &QAbstractButton::pressed,
            this,           &ChannelTuner::slotToolButton);
}

ChannelTuner::~ChannelTuner()
{
    delete ui;
}

void ChannelTuner::slotToolButton()
{
    auto dialog = new ChannelConfigurationDialog(this);

    dialog->exec();
}

double ChannelTuner::amplitude() const
{
    return ui->doubleSpinBoxAmplitude->value();
}

double ChannelTuner::deviation() const
{
    return ui->doubleSpinBoxDeviation->value();
}

QString ChannelTuner::name() const
{
    // return ui->;
}

MeasuredValueDescription ChannelTuner::description() const
{

}

//!< Amplitude + deviation * random();
double ChannelTuner::randomValue() const
{
    auto randVal = rand() % 1000;
    double randFactor = pow(-1.0, double(randVal % 2)) * (double)randVal / 1000.0;

    return (ui->doubleSpinBoxAmplitude->value() + ui->doubleSpinBoxDeviation->value() * randFactor);
}
