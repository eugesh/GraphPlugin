#include "qcphaircomb.h"

QCPHairComb::QCPHairComb(QCustomPlot *cplot) // QCPAxis *keyAxis, QCPAxis *valueAxis)
    : QCPAbstractPlottable1D<QCPVectorData>(cplot->xAxis, cplot->yAxis)
{
    QCPGraphDataContainer points1, points2;
    m_scatter1 = cplot->addGraph(cplot->xAxis, cplot->yAxis);
    m_scatter1->setLineStyle(QCPGraph::lsNone);
    m_scatter1->setScatterStyle(QCPScatterStyle::ssTriangle);
    m_scatter2 = cplot->addGraph(cplot->xAxis, cplot->yAxis);
    m_scatter2->setLineStyle(QCPGraph::lsNone);
    m_scatter2->setScatterStyle(QCPScatterStyle::ssDot);
}

void QCPHairComb::addData(const QVector<double> &keys1, const QVector<double> &values1,
                          const QVector<double> &keys2, const QVector<double> &values2,
                          bool alreadySorted0)
{
    m_scatter1->addData(keys1, values1, alreadySorted0);
    m_scatter2->addData(keys2, values2, alreadySorted0);
}

void QCPHairComb::addData(double key1, double value1, double key2, double value2)
{
    m_scatter1->addData(key1, value1);
    m_scatter2->addData(key2, value2);
}
