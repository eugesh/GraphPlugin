#ifndef SIMULATORMAINWINDOW_H
#define SIMULATORMAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class QFormLayout;
class GraphPluginConfig;
class GraphInterface;
class QPluginLoader;
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
    void onConfigure();

private:
    // Read JSON to propose user comboboxes with lists of SI units
    bool readSiJSON(const QString &path);
    // Write JSONs after configuration process is finished
    bool writeConfigJSON(const QString &path) const;
    bool writeGraphJSON(const QString &path) const;
    bool writeGraphJSONs() const;

    void enableConfigure(bool isEnabled = false);
    bool loadGraphPlugin();
    bool unloadGraphPlugin();
    MeasuredValue currentValue(const QString &name) const;
    QVector<MeasuredValue> allCurrentValues() const;

private:
    Ui::MainWindow *ui;
    QPluginLoader *m_graphPluginLoader = nullptr;
    GraphInterface *m_graphInterface = nullptr;
    // Number of channels
    int m_channelNum = 0;
    QFormLayout *m_formLayout;
    State m_state;
    QTimer m_greqTimer;
    bool m_enableConfigure = false;
    GraphPluginConfig *m_config;
};

#endif // SIMULATORMAINWINDOW_H
