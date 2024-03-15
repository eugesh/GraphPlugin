#ifndef QCPHAIRCOMB_H
#define QCPHAIRCOMB_H

#include <qcustomplot.h>
#include <QObject>
#include <QLine>

class QCP_LIB_DECL QCPVectorData
{
public:
  QCPVectorData();
  QCPVectorData(double key, double value);

  inline double sortKey() const { return key; }
  inline static QCPGraphData fromSortKey(double sortKey) { return QCPGraphData(sortKey, 0); }
  inline static bool sortKeyIsMainKey() { return true; }

  inline double mainKey() const { return key; }
  inline double mainValue() const { return value; }

  inline QCPRange valueRange() const { return QCPRange(value, value); }

  // double key, value;
  // QPointF start, end;
  QPair<double, double> keyVal1, keyVal2;
};
Q_DECLARE_TYPEINFO(QCPVectorData, Q_PRIMITIVE_TYPE);

class QCPHairComb : public QCPAbstractPlottable1D<QLine>
{
    Q_OBJECT
public:
    QCPHairComb(QCPAxis *keyAxis, QCPAxis *valueAxis);
};

#endif // QCPHAIRCOMB_H
