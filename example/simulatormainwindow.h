#ifndef SIMULATORMAINWINDOW_H
#define SIMULATORMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QFormLayout;
class GraphInterface;
struct MeasuredValue;

class SimulatorMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SimulatorMainWindow(QWidget *parent = nullptr);
    ~SimulatorMainWindow();

signals:
    void newData(const MeasuredValue &value);

private slots:
    void addChannel();
    void removeChannel();
    void onRun();
    void onStop();

private:
    bool loadGraphPlugin();

private:
    Ui::MainWindow *ui;
    GraphInterface *graphInterface;
    // Number of channels
    int m_channelNum = 0;
    QFormLayout *m_formLayout;
};

#endif // SIMULATORMAINWINDOW_H
