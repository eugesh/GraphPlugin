#ifndef COMMON_H
#define COMMON_H

#include "qcustomplot.h"

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

/**
 * @brief The MeasuredValueDescription struct
 *
 */
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

/**
 * @brief The GraphDir enum
 * X and Y axis direction (e.g. RIGHT to X and UP for Y)
 */
enum GraphDir {
    LEFT,
    UP,
    RIGHT,
    DOWN
};

/**
 * @brief The GraphUpdateMode enum
 * Graph update method
 */
enum GraphUpdateMode {
    SHOW_ALL, /*! Show all points  */
    SHOW_LAST_N, /*! Show latest N points */
    DONT_UPDATE
};

enum GraphScaleType {
    LIN,
    LOG
};

enum GraphType {
    GraphScatter, /*! X(Y) */
    GraphPolar, /*! Ro(Phi) */
    GraphParametric, /*! X(t), Y(t) */
    GraphIntegral, /*! Yi+1 = Yi-1 + Yi */
    GraphColorMap /*! X is t, Y and Z is colored */
};

/**
 * @brief The GraphProperties struct
 * Properties of
 */
struct GraphProperties {
    QString name;
    QString x_name;
    QString y_name;
    QString z_name; //! For color maps and waterfalls.
    QString x_title;
    QString y_title;
    QString z_title;
    QString x_unit;
    QString y_unit;
    QString z_unit;
    QString x_phisical_quantity;
    QString y_phisical_quantity;
    QString z_phisical_quantity;
    GraphDir x_dir; //! Direction of X axis.
    GraphDir y_dir; //! Direction of Y axis.
    unsigned int total_N; //! Total number of stored points.
    unsigned int last_N_limit; //! For SHOW_LAST_N update method
    GraphUpdateMode update_mode; //! See \link GraphUpdateMode \endlink.
    GraphScaleType x_scale;
    GraphScaleType y_scale;
    GraphScaleType z_scale;
    QVector<int> channels; // e.g. 1, 2, 3, 4
    QColor color;
    GraphType graphType;
    QCPColorGradient::GradientPreset colorScale;
    QString indexName;
    // bool is_parametric = false;
    // bool is_integral = false;
};

QString ColorToNameConverter(const QColor &color);
QColor nameToColorConverter(const QString &name);
GraphType nameToGraphTypeConverter(const QString &name);
QString graphTypeToNameConverter(const GraphType &type);
QMultiMap<QString, MeasuredValueDescription> loadConfigJSON(const QString &pathToJSON);
QCPColorGradient::GradientPreset nameToColorScaleTypeConverter(const QString &name);
QString colorScaleTypeToNameConverter(const QCPColorGradient::GradientPreset &type);

#endif // COMMON_H
