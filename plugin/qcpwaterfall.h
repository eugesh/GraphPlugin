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
    void addData(uint64_t timestamp, const QList<double> &xVec, const QList<double> &yVec, Qt::Orientation orient = Qt::Vertical);

private:
    // Vector of timestamps
    QQueue<uint64_t> m_timeVector;
    // Default value
    double m_defValue = 255;
    // Index of latest added row or column
    int m_lastColumnIndex = 0;
    int m_lastRowIndex = 0;
};

class QCPWaterfallScale : public QCPColorScale
{
    Q_OBJECT
public:
    explicit QCPWaterfallScale(QCustomPlot *parentPlot);
    void setGradient(const QCPColorGradient::GradientPreset &preset);
};

#endif // QCPWATERFALL_H
