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
    void addData(uint64_t timestamp, const QList<double> &vector, Qt::Orientation orient = Qt::Vertical);

private:
    double m_defValue = 255;
    QList<bool> m_verticalFillMask;
    QList<bool> m_horizontalFillMask;
    int m_lastColumnIndex = 0;
    int m_lastRowIndex = 0;
};

#endif // QCPWATERFALL_H
