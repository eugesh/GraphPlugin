#include "channelconfigurationdialog.h"
#include "channeltuner.h"
#include "graphpluginconfig.h"
#include "ui_channeltuner.h"

#include <cmath>


ChannelTuner::ChannelTuner(int channelNumber, GraphPluginConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelTuner),
    m_config(config)
{
    ui->setupUi(this);

    ui->label->setText("Source " + QString::number(channelNumber));

    connect(ui->toolButton, &QAbstractButton::pressed,
            this,           &ChannelTuner::slotToolButton);
}

ChannelTuner::~ChannelTuner()
{
    delete m_dialog;
    delete ui;
}

void ChannelTuner::slotToolButton()
{
    if (!m_dialog)
        m_dialog = new ChannelConfigurationDialog(m_config, this);

    m_dialog->exec();
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
