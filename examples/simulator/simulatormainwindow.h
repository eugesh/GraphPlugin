#ifndef SIMULATORMAINWINDOW_H
#define SIMULATORMAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class QFormLayout;
class GraphInterface;
struct MeasuredValue;

enum State {
    STOP, RUN, PAUSE
};

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
    MeasuredValue currentValue(const QString &name) const;
    QVector<MeasuredValue> allCurrentValue() const;

private:
    Ui::MainWindow *ui;
    GraphInterface *graphInterface = nullptr;
    // Number of channels
    int m_channelNum = 0;
    QFormLayout *m_formLayout;
    State m_state;
    QTimer m_greqTimer;
};

#endif // SIMULATORMAINWINDOW_H