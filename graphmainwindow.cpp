#include "graphmainwindow.h"
#include "ui_graphmainwindow.h"

GraphMainWindow::GraphMainWindow(const QString &path2JSON, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphMainWindow)
{
    ui->setupUi(this);
}

GraphMainWindow::~GraphMainWindow()
{
    delete ui;
}

bool GraphMainWindow::loadCSV()
{

    return true;
}

void GraphMainWindow::loadCSVdialog()
{

}

void GraphMainWindow::saveCSVdialog()
{

}

void GraphMainWindow::addData(const QList<MeasuredValue> &packet)
{
    for (MeasuredValue val1 : packet)
        for (QString val2_name : m_valueNameXY[val1.name])
            for (MeasuredValue val2 : packet)
                if (val2_name == val2.name)
                    m_valueGraphMap[qMakePair(val1.name, val2.name)]->addData(val1.value, val2.value);
}
