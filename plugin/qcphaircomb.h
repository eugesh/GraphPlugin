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

  inline double sortKey() const { return keyVal1.first; }
  inline static QCPGraphData fromSortKey(double sortKey) { return QCPGraphData(sortKey, 0); }
  inline static bool sortKeyIsMainKey() { return true; }

  inline double mainKey() const { return keyVal1.first; }
  inline double mainValue() const { return keyVal1.second; }

  inline QCPRange valueRange() const { return QCPRange(keyVal1.second, keyVal1.second); }

  // double key, value;
  // QPointF start, end;
  QPair<double, double> keyVal1, keyVal2;
};
Q_DECLARE_TYPEINFO(QCPVectorData, Q_PRIMITIVE_TYPE);


class QCPHairCombGraph : public QCPGraph
{
    Q_OBJECT
public:
    QCPHairCombGraph(QCPAxis *keyAxis, QCPAxis *valueAxis);

    void addData(const QVector<double> &keys1, const QVector<double> &values1,
                 const QVector<double> &keys2, const QVector<double> &values2,
                 bool alreadySorted=false);
    void addData(double key1, double value1, double key2, double value2);

private:

};


class QCPHairComb : public QCPAbstractPlottable1D<QCPVectorData>
{
    Q_OBJECT
public:
    QCPHairComb(QCustomPlot *cp = nullptr); // QCPAxis *keyAxis, QCPAxis *valueAxis);

    // virtual methods of 1d plottable interface:
    virtual int dataCount() const override;
    virtual double dataMainKey(int index) const override;
    virtual double dataSortKey(int index) const override;
    virtual double dataMainValue(int index) const override;
    virtual QCPRange dataValueRange(int index) const override;
    virtual QPointF dataPixelPosition(int index) const override;
    virtual bool sortKeyIsMainKey() const override;
    virtual QCPDataSelection selectTestRect(const QRectF &rect, bool onlySelectable) const override;
    virtual int findBegin(double sortKey, bool expandedRange=true) const override;
    virtual int findEnd(double sortKey, bool expandedRange=true) const override;

    // reimplemented virtual methods:
    virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=nullptr) const override;
    virtual QCPPlottableInterface1D *interface1D() override { return this; }
    virtual void setMaxCount(int count);
    void clear();

    void addData(const QVector<double> &keys1, const QVector<double> &values1,
                 const QVector<double> &keys2, const QVector<double> &values2,
                 bool alreadySorted=false);
    void addData(double key1, double value1, double key2, double value2);

private:
    QCPGraph *m_scatter1, *m_scatter2;
};

#endif // QCPHAIRCOMB_H
