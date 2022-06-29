#ifndef DIALINDICATOR_H
#define DIALINDICATOR_H

#include <QDial>
#include <QObject>
#include <QWidget>

class DialIndicator : public QDial
{
    Q_OBJECT
public:
    explicit DialIndicator(QWidget *parent = nullptr);

    void mousePressEvent(QMouseEvent *me) override;
    void mouseMoveEvent(QMouseEvent *me) override;
    void mouseReleaseEvent(QMouseEvent *me) override;
};

#endif // DIALINDICATOR_H
