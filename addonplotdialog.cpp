#include "addonplotdialog.h"
#include "common.h"
#include "graphmainwindow.h"
#include "graphpluginconfig.h"
#include "ui_addonplotdialog.h"

AddOnPlotDialog::AddOnPlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddOnPlotDialog)
{
    ui->setupUi(this);

    connect(ui->xPhysQuantCbBox, &QComboBox::currentTextChanged, [&](const QString &text) {
        ui->xUnitComboBox->clear();
        for (auto measUnit :  m_config->auxMeasUnits(text)) {
            ui->xUnitComboBox->addItem(measUnit["name"].toString());
        }
    });

    connect(ui->yPhysQuantCbBox, &QComboBox::currentTextChanged, [&](const QString &text) {
        ui->yUnitComboBox->clear();
        for (auto measUnit :  m_config->auxMeasUnits(text)) {
            ui->yUnitComboBox->addItem(measUnit["name"].toString());
        }
    });
}

AddOnPlotDialog::~AddOnPlotDialog()
{
    delete ui;
}

void AddOnPlotDialog::setConfig(GraphPluginConfig *config)
{
    m_config = config;

    // Fill in Physical quantity Combo Boxes
    for (auto physValName : config->physicalValuesNames()) {
        ui->xPhysQuantCbBox->addItem(physValName);
        ui->yPhysQuantCbBox->addItem(physValName);
    }



}

void AddOnPlotDialog::setMeasValDesc(const QMap<QString, MeasuredValueDescription> &mvd)
{
    m_measValuesDesc = mvd;

    for (auto val : mvd.values()) {
        ui->xNameComboBox->addItem(val.name);
        ui->yNameComboBox->addItem(val.name);
    }
}

void AddOnPlotDialog::setGraphProperties(const GraphProperties &defaultProp)
{

}

QString AddOnPlotDialog::getCustomPlotName() const
{
    return ui->customPltNameLineEdit->text();
}

GraphProperties AddOnPlotDialog::getProp() const
{
    GraphProperties prop;

    prop.name = ui->graphNameLineEdit->text();
    prop.x_name = ui->xNameComboBox->currentText();
    prop.y_name = ui->yNameComboBox->currentText();
    prop.x_unit = ui->xUnitComboBox->currentText();
    prop.y_unit = ui->yUnitComboBox->currentText();
    prop.x_phisical_quantity = ui->xPhysQuantCbBox->currentText();
    prop.y_phisical_quantity = ui->yPhysQuantCbBox->currentText();
    prop.x_dir = ui->xDirComboBox->currentText().contains(tr("Вправо")) ? GraphDir::RIGHT : GraphDir::LEFT;
    prop.y_dir = ui->yDirComboBox->currentText().contains(tr("Вверх")) ? GraphDir::UP : GraphDir::DOWN;
    prop.total_N = static_cast<unsigned int>(ui->spinBox_MaxLastN->value());
    prop.last_N_limit = static_cast<unsigned int>(ui->spinBox_MaxN->value());
    prop.update_mode = ui->updateModeCbBox->currentText().contains(tr("Отображать все")) ? GraphUpdateMode::SHOW_ALL : GraphUpdateMode::SHOW_LAST_N;
    prop.x_scale = ui->xScaleTypeCbBox->currentText().contains(tr("Линейная")) ? GraphScaleType::LIN : GraphScaleType::LOG;
    prop.y_scale = ui->yScaleTypeCbBox->currentText().contains(tr("Линейная")) ? GraphScaleType::LIN : GraphScaleType::LOG;
    prop.color = nameToColorConverter(ui->colorComboBox->currentText());

    return prop;
}
