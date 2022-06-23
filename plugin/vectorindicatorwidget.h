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
    bool readJSON(const QString &path);

public slots:
    void addData(const QList<MeasuredValue> &vals);

private:
    Ui::VectorIndicatorWidget *ui;
};

#endif // VECTORINDICATORWIDGET_H
