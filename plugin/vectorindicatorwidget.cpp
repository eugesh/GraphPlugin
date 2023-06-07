#include "common.h"
#include "ehmath.h"
#include "vectorindicatorwidget.h"
#include "ui_vectorindicatorwidget.h"

#include <QStyleFactory>
#include <QTimer>


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
    palette.setColor(QPalette::WindowText, Qt::darkGreen);
    ui->dial->setAutoFillBackground(true);
    ui->dial->setPalette(palette);
    ui->dial->setStyle(style);
    ui->dial->show();
    auto theta = QChar(0x98, 0x03);
    ui->label_angle->setText(theta + ':');
    ui->label_mag->setText("A:");
    ui->comboBox->setVisible(false);
    ui->comboBox_2->setVisible(false);
    setEnabled(false);
}

VectorIndicatorWidget::~VectorIndicatorWidget()
{
    delete ui;
}

void VectorIndicatorWidget::setAngle(double angle)
{
    ui->dial->setValue(fitAngleIn360(angle - 180));
    ui->lcdNumberAlpha->display(fitAngleIn360(angle));
    if (m_autoDisableTimeout > 0)
        m_hideTimer->start(m_autoDisableTimeout);
}

double VectorIndicatorWidget::getAngle() const
{
    return fitAngleIn360(ui->dial->value() - 180);
}

void VectorIndicatorWidget::setMagnitude(double mag)
{
    m_magnitude = mag;
    ui->lcdNumberMag->display(mag);
    if (m_autoDisableTimeout > 0)
        m_hideTimer->start(m_autoDisableTimeout);
}

double VectorIndicatorWidget::getMagnitude() const
{
    return m_magnitude;
}

void VectorIndicatorWidget::setAutoDisableOnIdle(int timeout)
{
    m_autoDisableTimeout = timeout;

    if (! m_hideTimer)
        m_hideTimer = new QTimer;

    connect(m_hideTimer, &QTimer::timeout, [this] () {
        setEnabled(false);
    });

    m_hideTimer->start(timeout);
}

bool VectorIndicatorWidget::readJSON(const QString &path)
{

    return true;
}

void VectorIndicatorWidget::addData(const QList<MeasuredValue> &vals)
{

}
