#include "digitalboarditem.h"
#include "ui_digitalboarditem.h"

#include <QMap>
#include <cmath>

static const double EPS = 0e-10;
/**
 * @brief DigitalBoardItem::DigitalBoardItem
 * LCD indicator and combobox with list of measuring units.
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
    m_prevText = mainUnit;
    if (mainUnit.isEmpty() || mainUnit == "%") {
        ui->comboBox->setCurrentText(QString("\%1").arg("\"%\""));
        m_fixedMultiplier = 1.0;
        m_fixedOffset = 0.0;
        m_measUnitsMult.clear();
        m_measUnitsMult.insert(QString("%"), 1.0);
        m_measUnitsOffsets.clear();
        m_measUnitsOffsets.insert(QString("%"), 0.0);
    } else {
        m_fixedMultiplier = measUnitsMult[mainUnit];
        m_fixedOffset = measUnitsOffsets[mainUnit];
    }

    connect(ui->comboBox, &QComboBox::currentTextChanged, [&](const QString &text) {
        // Convert current value to main unit
        double val = (m_currVal - m_measUnitsOffsets.value(m_prevText)) * m_measUnitsMult.value(m_prevText);
        val += m_fixedOffset;
        val /= m_fixedMultiplier;
        // Set converted value to LCD
        setCurrentValue(val);
        // Remember unit name
        m_prevText = text;
    });
}

/**
 * @brief DigitalBoardItem::setCurrentValue
 * @param val set current value to display on LCD indicator.
 */
void DigitalBoardItem::setCurrentValue(double val)
{
    val -= m_fixedOffset;
    val *= m_fixedMultiplier;
    m_prevText = ui->comboBox->currentText();

    auto currMult = (fabs(m_measUnitsMult.value(m_prevText)) > EPS ? m_measUnitsMult.value(m_prevText) : 1.0);
    m_currVal = val / currMult +
                m_measUnitsOffsets.value(m_prevText);
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
