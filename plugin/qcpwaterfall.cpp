#include "qcpwaterfall.h"

QCPWaterfall::QCPWaterfall(QCPAxis *keyAxis, QCPAxis *valueAxis)
    : QCPColorMap(keyAxis, valueAxis)
{

}

/**
 * @brief QCPWaterfall::setSize
 * @param size size of Color Map
 * @param defVal value to show
 */
void QCPWaterfall::setSize(const QSize &size, double defVal)
{
    data()->setSize(size.width(), size.height());
    m_defValue = defVal;

    for (int i = 0; i < size.height(); ++i) {
        for (int j = 0; j < size.width(); ++j) {
            data()->setCell(j, i, defVal);
        }
    }
}

/**
 * @brief QCPWaterfall::addData
 * @param vector row r column
 * @param orient horizontal or vertical
 */
void QCPWaterfall::addData(uint64_t timestamp, const QList<double> &vector, Qt::Orientation orient)
{
    auto W = data()->keySize();
    auto H = data()->valueSize();
    m_timeVector.enqueue(timestamp);

    if (orient == Qt::Horizontal) {
        int pos = H > m_lastRowIndex ? m_lastRowIndex++ : H - 1;
        for (int i = 0; i < vector.size(); ++i) {
            data()->setCell(i, pos, vector[i]);
        }
        if (H == m_lastRowIndex) {
            data()->removeRow(0);
            m_timeVector.dequeue();
        }
    } else {
        int pos = W > m_lastColumnIndex ? m_lastColumnIndex++ : W - 1;
        for (int i = 0; i < vector.size(); ++i) {
            data()->setCell(pos, i, vector[i]);
        }
        if (W == m_lastColumnIndex) {
            data()->removeColumn(0);
            m_timeVector.dequeue();
        }
    }

    data()->setKeyRange(QCPRange(m_timeVector.first(), m_timeVector.last()));
    rescaleAxes();
}
