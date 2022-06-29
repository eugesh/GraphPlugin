#include "dialindicator.h"

DialIndicator::DialIndicator(QWidget *parent)
    : QDial(parent)
{

}

void DialIndicator::mousePressEvent(QMouseEvent *me)
{
    QWidget::mousePressEvent(me);
}

void DialIndicator::mouseMoveEvent(QMouseEvent *me)
{
    QWidget::mouseMoveEvent(me);
}

void DialIndicator::mouseReleaseEvent(QMouseEvent *me)
{
    QWidget::mouseReleaseEvent(me);
}
