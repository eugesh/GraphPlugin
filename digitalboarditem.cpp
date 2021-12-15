#include "digitalboarditem.h"
#include "ui_digitalboarditem.h"

#include <QMap>

DigitalBoardItem::DigitalBoardItem(const QString &name, const QMap<QString, double> &measUnitsMult,
                                   const QMap<QString, double> &measUnitsOffsets, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DigitalBoardItem)
{
    ui->setupUi(this);

    m_measUnitsMult = measUnitsMult;
    m_measUnitsOffsets = measUnitsOffsets;

    ui->label->setText(name);

    for (auto unit : measUnitsMult.keys()) {
        ui->comboBox->addItem(unit, measUnitsMult[unit]);
    }

    connect(ui->comboBox, &QComboBox::currentTextChanged, [&](const QString &text) {
        setCurrentValue((m_currVal - m_measUnitsOffsets[m_prevText]) * m_measUnitsMult[m_prevText]);
        m_prevText = text;
    });
}

void DigitalBoardItem::setCurrentValue(double val) {
    m_prevText = ui->comboBox->currentText();
    m_currVal = val / m_measUnitsMult[ui->comboBox->currentText()] + m_measUnitsOffsets[ui->comboBox->currentText()];
    updateValue();
}

void DigitalBoardItem::updateValue()
{
    ui->lcdNumber->display(m_currVal);
}

DigitalBoardItem::~DigitalBoardItem()
{
    delete ui;
}
