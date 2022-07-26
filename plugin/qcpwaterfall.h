/**
 * Implementation of Waterfall data visualizer
 */

#ifndef QCPWATERFALL_H
#define QCPWATERFALL_H

#include "qcustomplot.h"
#include <QObject>

class QCPWaterfall : public QCPColorMap
{
    Q_OBJECT
public:
    explicit QCPWaterfall(QCPAxis *keyAxis, QCPAxis *valueAxis);

    void setSize(const QSize &size, double defVal);
    void addData(uint64_t timestamp, const QList<double> &vector, Qt::Orientation orient);

private:
    double m_defValue = 255;
    QList<bool> m_verticalFillMask;
    QList<bool> m_horizontalFillMask;
    int m_lastColumnIndex;
    int m_lastRowIndex;
};

#endif // QCPWATERFALL_H
