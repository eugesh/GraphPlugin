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
    ui->dial->setMinimum(0);
    ui->dial->setMaximum(360);
    QStringList styleNames = QStyleFactory::keys();
    QStyle *style = QStyleFactory::create("Windows");
    QPalette palette = QPalette();
    // palette.setColor(QPalette::Window, qRgb(0, 0, 0));
    // palette.setColor(QPalette::Button, qRgb(255, 255, 255));
    // style->polish(palette);
    // palette.setColor(QPalette::Background, Qt::black);
    palette.setColor(QPalette::Button, Qt::darkGreen);
    palette.setColor(QPalette::Foreground, Qt::darkGreen);
    ui->dial->setAutoFillBackground(true);
    ui->dial->setPalette(palette);
    ui->dial->setStyle(style);
    ui->dial->show();
}

VectorIndicatorWidget::~VectorIndicatorWidget()
{
    delete ui;
}

void VectorIndicatorWidget::setAngle(double angle)
{
    ui->dial->setValue(fitAngleIn360(angle - 180));
    ui->lcdNumber->display(fitAngleIn360(angle));
}

double VectorIndicatorWidget::getAngle() const
{
    return fitAngleIn360(ui->dial->value() - 180);
}

void VectorIndicatorWidget::setMagnitude(double mag)
{
    m_magnitude = mag;
    ui->lcdNumber_2->display(mag);
}

double VectorIndicatorWidget::getMagnitude() const
{
    return m_magnitude;
}

bool VectorIndicatorWidget::readJSON(const QString &path)
{

    return true;
}

void VectorIndicatorWidget::addData(const QList<MeasuredValue> &vals)
{

}
