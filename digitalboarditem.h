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
    explicit DigitalBoardItem(const QString &name, const QMap<QString, double> &measUnitsMult, const QMap<QString,
                              double> &measUnitsOffsets, QWidget *parent = nullptr);
    ~DigitalBoardItem();

    void setCurrentValue(double val);

    void updateValue();

private:
    Ui::DigitalBoardItem *ui;
    QMap<QString, double> m_measUnitsMult;
    QMap<QString, double> m_measUnitsOffsets;
    double m_currVal;
};

#endif // DIGITALBOARDITEM_H
