#include "addonplotdialog.h"
#include "common.h"
#include "graphmainwindow.h"
#include "graphpluginconfig.h"
#include "ui_addonplotdialog.h"

AddOnPlotDialog::AddOnPlotDialog(int channelsNum, QWidget *parent) :
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

    for (int i = 0; i < channelsNum; ++i) {
        QCheckBox *chbx = new QCheckBox(this);
        chbx->setText(QString("%1").arg(i));
        labelChannels->layout()->addWidget(chbx);
        ui->labelChannels->layout()->itemAt(i + 1)->setChecked(false);
        ui->labelChannels->layout()->itemAt(i + 1)->setEnabled(true);
    }
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

void AddOnPlotDialog::setProp(const GraphProperties &prop)
{
    ui->graphNameLineEdit->clear();
    ui->graphNameLineEdit->insert(prop.name);
    ui->xNameComboBox->setCurrentText(prop.x_name);
    ui->yNameComboBox->setCurrentText(prop.y_name);
    ui->xTitleEdit->clear();
    ui->xTitleEdit->insert(prop.x_title);
    ui->yTitleEdit->clear();
    ui->yTitleEdit->insert(prop.y_title);
    ui->xUnitComboBox->setCurrentText(prop.x_unit);
    ui->yUnitComboBox->setCurrentText(prop.y_unit);

    ui->xPhysQuantCbBox->setCurrentText(prop.x_phisical_quantity);
    ui->yPhysQuantCbBox->setCurrentText(prop.y_phisical_quantity);

    ui->xDirComboBox->setCurrentText(prop.x_dir == GraphDir::RIGHT ? tr("Вправо") : tr("Влево"));
    ui->yDirComboBox->setCurrentText(prop.y_dir == GraphDir::UP ? tr("Вверх") : tr("Вниз"));

    ui->spinBox_MaxLastN->setValue(prop.total_N);
    ui->spinBox_MaxN->setValue(prop.last_N_limit);

    ui->updateModeCbBox->setCurrentText(prop.update_mode == GraphUpdateMode::SHOW_ALL ? tr("Отображать все") : tr("Последние N"));
    ui->xScaleTypeCbBox->setCurrentText(prop.x_scale == GraphScaleType::LIN ? tr("Линейная") : tr("Логарифмическая"));
    ui->yScaleTypeCbBox->setCurrentText(prop.y_scale == GraphScaleType::LIN ? tr("Линейная") : tr("Логарифмическая"));

    for (auto ch : prop.channels) {
        if (ch < ui->labelChannels->layout()->count()) {
            ui->labelChannels->layout()->itemAt(ch)->setChecked(true);
            ui->labelChannels->layout()->itemAt(ch)->setEnabled(true);
        }
    }

    ui->colorComboBox->setCurrentText(ColorToNameConverter(prop.color));
}

GraphProperties AddOnPlotDialog::getProp() const
{
    GraphProperties prop;

    prop.name = ui->graphNameLineEdit->text();
    prop.x_name = ui->xNameComboBox->currentText();
    prop.y_name = ui->yNameComboBox->currentText();
    prop.x_title = ui->xTitleEdit->text();
    prop.y_title = ui->yTitleEdit->text();
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

QVector<int> AddOnPlotDialog::channels() const
{
    QVector<int> chls;

    if (ui->checkBoxCh1.checked()) chls << 1;
    if (ui->checkBoxCh2.checked()) chls << 2;
    if (ui->checkBoxCh3.checked()) chls << 3;
    if (ui->checkBoxCh4.checked()) chls << 4;

    return chls;
}
