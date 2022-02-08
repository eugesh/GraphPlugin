#include "channelconfigurationdialog.h"
#include "graphpluginconfig.h"
#include "ui_channelconfigurationdialog.h"


ChannelConfigurationDialog::ChannelConfigurationDialog(GraphPluginConfig *config, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChannelConfigurationDialog),
    m_config(config)
{
    ui->setupUi(this);

    connect(ui->comboBox_physcs, &QComboBox::currentTextChanged,
            this, &ChannelConfigurationDialog::onComboBoxPhyscsChanged);

    fillForm();
}

ChannelConfigurationDialog::~ChannelConfigurationDialog()
{
    delete ui;
}

void ChannelConfigurationDialog::onComboBoxPhyscsChanged(const QString &physValName)
{
    ui->comboBox_units->clear();

    for (auto unitName : m_config->measurementUnits(physValName))
        ui->comboBox_units->addItem(unitName["name_ru"].toString());
}

void ChannelConfigurationDialog::fillForm()
{
    for (auto name : m_config->physicalValuesNames())
        ui->comboBox_physcs->addItem(name);
}
