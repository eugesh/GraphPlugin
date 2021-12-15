#include "addonplotdialog.h"
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

void AddOnPlotDialog::setGraphProperties(const GraphProperties &defaultProp)
{

}

GraphProperties AddOnPlotDialog::getProp() const
{
    GraphProperties prop;

    prop.name = ui->pltNameLineEdit->text();
    prop.x_name = ui->xNameComboBox->currentText();
    prop.y_name;
    prop.x_unit;
    prop.y_unit;
    prop.x_phisical_quantity;
    prop.y_phisical_quantity;
    prop.x_dir;
    prop.y_dir;
    prop.total_N;
    prop.last_N_limit;
    prop.update_mode;
    prop.x_scale;
    prop.y_scale;
    prop.color;

    QComboBox *yNameComboBox;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_5;
    QComboBox *xDirComboBox;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_6;
    QComboBox *yDirComboBox;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_7;
    QLineEdit *xTitleComboBox;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_8;
    QLineEdit *yTitleComboBox;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_9;
    QComboBox *xPhysQuantCbBox;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_10;
    QComboBox *yPhysQuantCbBox;
    QHBoxLayout *horizontalLayout_17;
    QLabel *label_18;
    QComboBox *xUnitComboBox;
    QHBoxLayout *horizontalLayout_16;
    QLabel *label_17;
    QComboBox *yUnitComboBox;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_11;
    QSpinBox *spinBox_MaxN;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_12;
    QSpinBox *spinBox_MaxLastN;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_14;
    QComboBox *xScaleTypeCbBox;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_13;
    QComboBox *yScaleTypeCbBox;
    QHBoxLayout *horizontalLayout_15;
    QLabel *label_16;
    QComboBox *updateModeCbBox;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_15;
    QComboBox *colorComboBox;

    return prop;
}
