#include "common.h"
#include "vectorindicatorwidget.h"
#include "ui_vectorindicatorwidget.h"

#include <QStyleFactory>

inline double fitAngleIn360(double angle) {
    double outAngle = angle;
    while (outAngle > 360) {
        outAngle -= 360;
    }

    while (outAngle < 0) {
        outAngle += 360;
    }

    return outAngle;
}

VectorIndicatorWidget::VectorIndicatorWidget(const QString &name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VectorIndicatorWidget)
{
    ui->setupUi(this);

    setObjectName(tr("%1%2").arg(name).arg("BoardItem"));

    ui->label->setText(name);

    ui->dial->setWrapping(true);
    ui->dial->setNotchesVisible(true);
    QStringList styleNames = QStyleFactory::keys();
    QStyle *style = QStyleFactory::create("Windows");
    QPalette palette = QPalette();
    style->polish(palette);
    ui->dial->setStyle(style);
}

VectorIndicatorWidget::~VectorIndicatorWidget()
{
    delete ui;
}

void VectorIndicatorWidget::setAngle(double angle)
{
    ui->dial->setValue(fitAngleIn360(270 - angle));
}

double VectorIndicatorWidget::getAngle() const
{
    return fitAngleIn360(270 - ui->dial->value());
}

bool VectorIndicatorWidget::readJSON(const QString &path)
{

}

void VectorIndicatorWidget::addData(const QList<MeasuredValue> &vals)
{

}
