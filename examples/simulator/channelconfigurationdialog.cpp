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
        ui->comboBox_units->addItem(unitName.values("name_ru").first().toString(),
                                    unitName.values("name").first().toString());
}

QString ChannelConfigurationDialog::measurementValueName() const
{
    return ui->lineEdit->text();
}

QString ChannelConfigurationDialog::description() const
{
    return ui->lineEdit_description->text();
}

QString ChannelConfigurationDialog::physicalValueName() const
{
    return ui->comboBox_physcs->currentText();
}

QString ChannelConfigurationDialog::measurementUnitName() const
{
    return ui->comboBox_units->currentData().toString();
}

QString ChannelConfigurationDialog::measurementUnitNameTr() const
{
    return ui->comboBox_units->currentText();
}

void ChannelConfigurationDialog::fillForm()
{
    for (auto name : m_config->physicalValuesNames())
        ui->comboBox_physcs->addItem(name);
}

bool ChannelConfigurationDialog::addTimePlotChecked() const
{
    return ui->checkBox_addPlot->isChecked();
}

void ChannelConfigurationDialog::setCurrentPhysicalValue(const QString &name)
{
    ui->comboBox_physcs->setCurrentText(name);
}

void ChannelConfigurationDialog::setCurrentMeasurementUnit(const QString &name)
{
    ui->comboBox_units->setCurrentText(name);
}

void ChannelConfigurationDialog::setName(const QString &name)
{
    ui->lineEdit->setText(name);
}

void ChannelConfigurationDialog::setDescription(const QString &name)
{
    ui->lineEdit_description->setText(name);
}

void ChannelConfigurationDialog::setPhysicalValues(const QStringList &names)
{
    ui->comboBox_physcs->insertItems(0, names);
}

bool ChannelConfigurationDialog::isUseStandardPlot() const
{
    return ui->checkBox_addPlot->isChecked();
}

/*void ChannelConfigurationDialog::setMeasurementUnits(const QStringList &names)
{
    for (auto name : names)
        ui->comboBox_units->insertItem(name.name);
}*/
