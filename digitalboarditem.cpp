#include "digitalboarditem.h"
#include "ui_digitalboarditem.h"

#include <QMap>

DigitalBoardItem::DigitalBoardItem(const QString &name, const QMap<QString, double> &measUnits, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DigitalBoardItem)
{
    ui->setupUi(this);

    ui->label->setText(name);

    for (auto unit : measUnits.keys()) {
        ui->comboBox->addItem(unit, measUnits[unit]);
    }

    connect(ui->comboBox, &QComboBox::currentTextChanged, [&](const QString &text) {
        m_currVal *= measUnits[text];
        updateValue();
    });
}

void DigitalBoardItem::setCurrentValue(double val) {
    m_currVal = val;
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
