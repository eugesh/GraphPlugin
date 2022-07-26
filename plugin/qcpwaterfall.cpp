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
}

/**
 * @brief QCPWaterfall::addData
 * @param vector row r column
 * @param orient horizontal or vertical
 */
void QCPWaterfall::addData(uint64_t timestamp, const QList<double> &vector, Qt::Orientation orient)
{
    data()->keySize();
    data()->valueSize();

    if (orient == Qt::Horizontal) {
        int pos = data()->valueSize() > m_lastRowIndex ? m_lastRowIndex++ : data()->valueSize();
        for (int i = 0; i < vector.size(); ++i) {
            data()->setCell(pos, i, vector[i]);
        }
    } else {
        int pos = data()->keySize() > m_lastColumnIndex ? m_lastColumnIndex++ : data()->valueSize();
        for (int i = 0; i < vector.size(); ++i) {
            data()->setCell(i, pos, vector[i]);
        }
    }
}
