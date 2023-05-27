#include "dialindicator.h"

/**
 * @brief DialIndicator::DialIndicator
 * Gauge like indicator with arrow, dial and two indicators: angular and magnitude.
 * @param parent parent widget, optional
 */
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
