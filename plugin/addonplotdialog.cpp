#include "addonplotdialog.h"
#include "graphmainwindow.h"
#include "graphpluginconfig.h"
#include "ui_addonplotdialog.h"

using namespace Graph;

/**
 * @brief AddOnPlotDialog::AddOnPlotDialog
 * Dialog for adding new graph on plot area.
 * @param parent parent class, optional.
 * @param channelsNum number of chanels (todo).
 */
AddOnPlotDialog::AddOnPlotDialog(QWidget *parent, int channelsNum) :
    QDialog(parent),
    ui(new Ui::AddOnPlotDialog)
{
    ui->setupUi(this);

    connect(ui->xPhysQuantCbBox, &QComboBox::currentTextChanged, this, [&](const QString &text) {
        ui->xUnitComboBox->clear();
        for (auto &measUnit :  m_config->measurementUnits(text)) {
            ui->xUnitComboBox->addItem(measUnit.value("name").toString());
        }
    });

    connect(ui->yPhysQuantCbBox, &QComboBox::currentTextChanged, this, [&](const QString &text) {
        ui->yUnitComboBox->clear();
        for (auto &measUnit :  m_config->measurementUnits(text)) {
            ui->yUnitComboBox->addItem(measUnit.value("name").toString());
        }
    });

    for (int i = 1; i <= channelsNum; ++i) {
        QCheckBox *chbx = new QCheckBox(this);
        chbx->setText(QString("%1").arg(i));
        ui->horizontalLayoutCh->addWidget(chbx);
        auto obj = qobject_cast<QCheckBox*> (ui->horizontalLayoutCh->itemAt(i)->widget());
        if (obj) {
            obj->setChecked(false);
            obj->setEnabled(true);
        }
    }
}

AddOnPlotDialog::~AddOnPlotDialog()
{
    delete ui;
}

/**
 * @brief AddOnPlotDialog::setConfig
 * @param config pointer to \link GraphPluginConfig \endlink
 */
void AddOnPlotDialog::setConfig(GraphPluginConfig *config)
{
    m_config = config;

    // Fill in Physical quantity Combo Boxes
    for (auto &physValName : config->physicalValuesNames()) {
        ui->xPhysQuantCbBox->addItem(physValName);
        ui->yPhysQuantCbBox->addItem(physValName);
    }
}

/**
 * @brief AddOnPlotDialog::setMeasValDesc
 * @param mvd multimap Measuring Value Name <-> Measuring Value's Description
 */
void AddOnPlotDialog::setMeasValDesc(const QMultiMap<QString, Graph::MeasuredValueDescription> &mvd)
{
    m_measValuesDesc = mvd;

    const auto vals = mvd.values();
    for (auto &val : vals) {
        ui->xNameComboBox->addItem(val.name);
        ui->yNameComboBox->addItem(val.name);
    }

    // Add "time"
    Graph::MeasuredValueDescription timeVal;
    timeVal.desc = "time";
    timeVal.desc_ru = tr("время");
    timeVal.name = "time";
    timeVal.physQuant = "time";
    timeVal.unit = "second";
    m_measValuesDesc.insert("time", timeVal);
    ui->xNameComboBox->addItem("time");
}

void AddOnPlotDialog::setGraphProperties(const Graph::GraphProperties &defaultProp)
{

}

/**
 * @brief AddOnPlotDialog::getCustomPlotName
 * @return name of plotting area.
 */
QString AddOnPlotDialog::getCustomPlotName() const
{
    return ui->customPltNameLineEdit->text();
}

/**
 * @brief AddOnPlotDialog::setTitle
 * @param name title of dialog.
 */
void AddOnPlotDialog::setTitle(const QString& name)
{
    ui->mainLabel->setText(name);
}

/**
 * @brief AddOnPlotDialog::setProp
 * @param prop \link GraphProperties \endlink
 */
void AddOnPlotDialog::setProp(const Graph::GraphProperties &prop)
{
    ui->customPltNameLineEdit->setText(prop.name);
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

    ui->spinBox_MaxLastN->setValue(prop.last_N_limit);
    ui->spinBox_MaxN->setValue(prop.total_N);

    ui->updateModeCbBox->setCurrentText(prop.update_mode == GraphUpdateMode::SHOW_ALL ? tr("Отображать все") : tr("Последние N"));
    ui->xScaleTypeCbBox->setCurrentText(prop.x_scale == GraphScaleType::LIN ? tr("Линейная") : tr("Логарифмическая"));
    ui->yScaleTypeCbBox->setCurrentText(prop.y_scale == GraphScaleType::LIN ? tr("Линейная") : tr("Логарифмическая"));
    ui->graphTypeCbBox->setCurrentIndex(static_cast<int>(prop.graphType));
    // ui->parametricCurveCheckBox->setChecked(prop.is_parametric ? true : false);
    // ui->integrateCheckBox->setChecked(prop.is_integral ? true : false);

    for (auto ch : prop.channels) {
        if (ch < ui->horizontalLayoutCh->count()) {
            auto obj = qobject_cast<QCheckBox*> (ui->horizontalLayoutCh->itemAt(ch)->widget());
            if (obj) {
                obj->setChecked(true);
                obj->setEnabled(true);
            }
        }
    }

    ui->colorComboBox->setCurrentText(ColorToNameConverter(prop.color));
}

Graph::GraphProperties AddOnPlotDialog::getProp() const
{
    Graph::GraphProperties prop;

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
    prop.color = Graph::nameToColorConverter(ui->colorComboBox->currentText());
    prop.graphType = static_cast<GraphType> (ui->graphTypeCbBox->currentIndex());
    // prop.is_parametric = ui->parametricCurveCheckBox->isChecked();
    // prop.is_integral = ui->integrateCheckBox->isChecked();

    for (int i = 1; i < ui->horizontalLayoutCh->count(); ++i) {
        auto obj = qobject_cast<QCheckBox*> (ui->horizontalLayoutCh->itemAt(i)->widget());
        if (obj) {
            prop.channels << i;
        }
    }

    return prop;
}

QVector<int> AddOnPlotDialog::channels() const
{
    QVector<int> chls;

    for (int ch = 1; ch < ui->horizontalLayoutCh->count(); ch++) {
        auto obj = qobject_cast<QCheckBox*> (ui->horizontalLayoutCh->itemAt(ch)->widget());
        if (obj) {
            chls << ch;
        }
    }

    return chls;
}
