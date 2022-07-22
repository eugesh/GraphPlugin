#ifndef COMMON_H
#define COMMON_H

#include <qnamespace.h>
#include <QColor>
#include <QVector>

enum GraphPluginMode {
    GRAPH_DATA_SYNCH,         //!< Data represents one packet, every value arrives synchronously (Telemetry packet mode)
    GRAPH_DATA_ASYNC          //!< Values arrives asynchronously () // Not implemented yet
};

enum : int {
    ClipboardTextRole = Qt::UserRole + 1,
    DataRole = Qt::UserRole + 2,
    PhysQuantRole = Qt::UserRole + 3
};

struct MeasuredValueDescription {
    QString name;
    QString desc;
    QString desc_ru;
    QString physQuant;
    QString unit;
    QString unit_rus;
    QString symbol;
    QString symbol_rus;
};

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

enum GraphType {
    GraphScatter,
    GraphParametric,
    GraphIntegral,
    GraphColorMap
};

struct GraphProperties {
    QString name;
    QString x_name;
    QString y_name;
    QString z_name; // For color maps and waterfalls
    QString x_title;
    QString y_title;
    QString z_title;
    QString x_unit;
    QString y_unit;
    QString z_unit;
    QString x_phisical_quantity;
    QString y_phisical_quantity;
    QString z_phisical_quantity;
    GraphDir x_dir;
    GraphDir y_dir;
    unsigned int total_N;
    unsigned int last_N_limit;
    GraphUpdateMode update_mode;
    GraphScaleType x_scale;
    GraphScaleType y_scale;
    GraphScaleType z_scale;
    QVector<int> channels; // e.g. 1, 2, 3, 4
    QColor color;
    GraphType graphType;
    // bool is_parametric = false;
    // bool is_integral = false;
};

QString ColorToNameConverter(const QColor &color);
QColor nameToColorConverter(const QString &name);
GraphType nameToGraphTypeConverter(const QString &name);
QString graphTypeToNameConverter(const GraphType &type);
QMultiMap<QString, MeasuredValueDescription> loadConfigJSON(const QString &pathToJSON);

#endif // COMMON_H
