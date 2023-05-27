#ifndef VECTORINDICATORSBOARD_H
#define VECTORINDICATORSBOARD_H

#include "common.h"
#include "graph_interface.h"

#include <QMainWindow>
#include <QMap>
#include <QString>

using namespace Graph;

namespace Ui {
class VectorIndicatorsBoard;
}

class VectorIndicatorWidget;
class GraphPluginConfig;
struct GraphProperties;


class VectorIndicatorsBoard : public QMainWindow
{
    Q_OBJECT

public:
    explicit VectorIndicatorsBoard(QWidget *parent = nullptr);
    ~VectorIndicatorsBoard();

    // Call at first!
    bool setValuesDescriptions(const QMultiMap<QString, Graph::MeasuredValueDescription> &mvd);
    void setConfig(GraphPluginConfig *config);

    // Apply the last custom settings
    bool initFromJSON(const QString &pathToJSON);
    void setAutoDisableOnIdle(int timeout);

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    void addData(const QList<MeasuredValue> &vals);
    void addNewIndicator(const Graph::GraphProperties &prop);
    void onSaveJsonAs();
    void onRemoveJson();

signals:
    void deleteMe();

private:
    bool readJSON(const QString &path);
    bool saveJSON(const QString &path) const;
    bool restoreBoardGeometry();
    bool saveBoardGeometry();

private:
    Ui::VectorIndicatorsBoard *ui;

    QString m_JSONPath;
    bool m_hasUpdate;
    GraphPluginConfig *m_config;
    QMultiMap<QString, Graph::MeasuredValueDescription> m_measValDescMap;
    QMap<QString, VectorIndicatorWidget*> m_items;
    // Key - abscissa, value - widget with vector indicator
    QMap<QString, QDockWidget*> m_itemsDocks;
    QList<QString> m_activeSensorsNames;
    // Key - abscissa, value - ordinate
    QMap<QString, QString> m_xyMap;
    QMap<QString, Graph::GraphProperties> m_namePropMap;
    QMap<std::pair<QString, QString>, QString> m_xyNameMap;
    QMap<QString, Graph::GraphProperties> m_properties;
    int m_autoHideTimeout = -1;
};

#endif // VECTORINDICATORSBOARD_H
