#ifndef GRAPH_COMMON_H
#define GRAPH_COMMON_H

#include <QColor>
#include <QString>
#include <QVector>

enum GraphDir {
    LEFT,
    UP,
    RIGHT,
    DOWN
};

enum GraphUpdateMode {
    SHOW_ALL,
    SHOW_LAST_N,
    DONT_UPDATE
};

enum GraphScaleType {
    LIN,
    LOG
};

// Unique ID of measured value
struct valID {
    // Number of channel
    int chNumber;
    // Name of physical quantity
    QString name;
};

// Unique ID of graph figure
struct GraphID {
    QString graphName;
    // Number of channel
    int chNumber;
    // Names of axes
    QString xName;
    QString yName;
};

struct GraphProperties {
    QString name;
    QString x_name;
    QString y_name;
    QString x_title;
    QString y_title;
    QString x_unit;
    QString y_unit;
    QString x_phisical_quantity;
    QString y_phisical_quantity;
    GraphDir x_dir;
    GraphDir y_dir;
    unsigned int total_N;
    unsigned int last_N_limit;
    GraphUpdateMode update_mode;
    GraphScaleType x_scale;
    GraphScaleType y_scale;
    QVector<int> channels; // e.g. 1, 2, 3, 4
    QColor color;
    // Default constructor
    GraphProperties() {
        x_name = "time";
        x_title = QT_TR_NOOP("время");
        x_unit = "second";
        x_phisical_quantity = "time";
        x_dir = LEFT;
        y_dir = UP;
        total_N = 1000;
        last_N_limit = 100;
        update_mode = SHOW_ALL;
        x_scale = LIN;
        y_scale = LIN;
        channels = {};
        color = Qt::red;
    }
};

// Common functions
bool saveJSON(const QString &pathToJSON, const QList<GraphProperties> &properties, const QString &windowTitle);

#endif // GRAPH_COMMON_H
