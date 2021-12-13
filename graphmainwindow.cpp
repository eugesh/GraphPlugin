#include "graphmainwindow.h"
#include "ui_graphmainwindow.h"

GraphMainWindow::GraphMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphMainWindow)
{
    ui->setupUi(this);
}

GraphMainWindow::~GraphMainWindow()
{
    delete ui;
}
