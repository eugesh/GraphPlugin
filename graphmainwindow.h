#ifndef GRAPHMAINWINDOW_H
#define GRAPHMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class GraphMainWindow;
}

class GraphMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphMainWindow(QWidget *parent = nullptr);
    ~GraphMainWindow();

private:
    Ui::GraphMainWindow *ui;
};

#endif // GRAPHMAINWINDOW_H
