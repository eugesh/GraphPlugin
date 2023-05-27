#ifndef DIGITALBOARDITEM_H
#define DIGITALBOARDITEM_H

#include <QMap>
#include <QWidget>

namespace Ui {
class DigitalBoardItem;
}

class DigitalBoardItem : public QWidget
{
    Q_OBJECT

public:
    explicit DigitalBoardItem(const QString &name, const QString &mainUnit, const QMap<QString, double> &measUnitsMult, const QMap<QString,
                              double> &measUnitsOffsets, QWidget *parent = nullptr);
    ~DigitalBoardItem();
    void setCurrentValue(double val);

private:
    void updateValue();

private:
    Ui::DigitalBoardItem *ui;
    QMap<QString, double> m_measUnitsMult;
    QMap<QString, double> m_measUnitsOffsets;
    double m_currVal;
    // Previous meausurement unit name
    QString m_prevText;
    // In the case when data comes not in SI unit
    double m_fixedMultiplier = 1.0;
    double m_fixedOffset = 0.0;
};

#endif // DIGITALBOARDITEM_H
