#include "digitalboarditem.h"
#include "ui_digitalboarditem.h"

#include <QMap>

/**
 * @brief DigitalBoardItem::DigitalBoardItem
 * @param name name of item;
 * @param mainUnit unit of the measuring value;
 * @param measUnitsMult multiplier;
 * @param measUnitsOffsets offset from the base unit;
 * @param parent parent of the item
 */
DigitalBoardItem::DigitalBoardItem(const QString &name, const QString &mainUnit, const QMap<QString, double> &measUnitsMult,
                                   const QMap<QString, double> &measUnitsOffsets, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DigitalBoardItem)
{
    ui->setupUi(this);

    setObjectName(tr("%1%2").arg(name).arg("BoardItem"));

    m_measUnitsMult = measUnitsMult;
    m_measUnitsOffsets = measUnitsOffsets;

    ui->label->setText(name);

    for (auto unit : measUnitsMult.keys()) {
        ui->comboBox->addItem(unit, measUnitsMult[unit]);
    }

    ui->comboBox->setCurrentText(mainUnit);
    m_fixedMultiplier = measUnitsMult[mainUnit];
    m_fixedOffset = measUnitsOffsets[mainUnit];

    connect(ui->comboBox, &QComboBox::currentTextChanged, [&](const QString &text) {
        // Convert current value to main unit
        double val = (m_currVal - m_measUnitsOffsets[m_prevText]) * m_measUnitsMult[m_prevText];
        val += m_fixedOffset;
        val *= m_fixedMultiplier;
        // Set converted value to LCD
        setCurrentValue(val);
        // Remember unit name
        m_prevText = text;
    });
}

void DigitalBoardItem::setCurrentValue(double val)
{
    val -= m_fixedOffset;
    val /= m_fixedMultiplier;
    m_prevText = ui->comboBox->currentText();
    m_currVal = val / m_measUnitsMult[m_prevText] +
                m_measUnitsOffsets[m_prevText];
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
