#ifndef DIGITALBOARDITEM_H
#define DIGITALBOARDITEM_H

#include <QWidget>

namespace Ui {
class DigitalBoardItem;
}

class DigitalBoardItem : public QWidget
{
    Q_OBJECT

public:
    explicit DigitalBoardItem(const QString &name, const QMap<QString, double> &measUnits, QWidget *parent = nullptr);
    ~DigitalBoardItem();

    void setCurrentValue(double val);

    void updateValue();

private:
    Ui::DigitalBoardItem *ui;
    double m_currVal;
};

#endif // DIGITALBOARDITEM_H
