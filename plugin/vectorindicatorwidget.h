#ifndef VECTORINDICATORWIDGET_H
#define VECTORINDICATORWIDGET_H

#include <QWidget>

struct MeasuredValueDescription;
struct MeasuredValue;

namespace Ui {
class VectorIndicatorWidget;
}

class VectorIndicatorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VectorIndicatorWidget(const QString &name, QWidget *parent = nullptr);
    ~VectorIndicatorWidget();
    void setAngle(double angle);
    double getAngle() const;
    void setMagnitude(double mag);
    double getMagnitude() const;
    bool readJSON(const QString &path);
    void setAutoDisableOnIdle(int timeout);

public slots:
    void addData(const QList<MeasuredValue> &vals);

private:
    Ui::VectorIndicatorWidget *ui;
    double m_magnitude = 0;
    int m_autoDisableTimeout = -1; // -1 - not disable
    QTimer *m_hideTimer = nullptr;
};

#endif // VECTORINDICATORWIDGET_H
