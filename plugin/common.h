#ifndef COMMON_H
#define COMMON_H

#include "../QCustomPlot/qcustomplot.h"

#include <qnamespace.h>
#include <QColor>
#include <QVector>

namespace Graph {

enum GraphPluginMode {
    GRAPH_DATA_SYNCH,         //!< Data represents one packet, every value arrives synchronously (Telemetry packet mode).
    GRAPH_DATA_ASYNC          //!< Values arrives asynchronously (). // Not implemented yet
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
    QString name; //! Name of measured value.
    QString desc; //! Description.
    QString desc_ru; //! Description in Russian.
    QString physQuant; //! Physical quantity.
    QString unit; // Measurement unit.
    QString unit_rus; // // Measurement unit in Russian.
    QString symbol; // Short name of Measurement unit, e.g. [m].
    QString symbol_rus; // Short name of Measurement unit in Russian, e.g. [м].
};

/**
 * @brief The GraphDir enum
 * X and Y axis direction (e.g. RIGHT to X and UP for Y).
 */
enum GraphDir {
    LEFT,
    UP,
    RIGHT,
    DOWN
};

/**
 * @brief The GraphUpdateMode enum
 * Graph update method.
 */
enum GraphUpdateMode {
    SHOW_ALL, /*! Show all points  */
    SHOW_LAST_N, /*! Show latest N points */
    DONT_UPDATE
};

enum GraphScaleType {
    LIN, //! Linear scale
    LOG //! Logarithmic scale
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
 * Common properties of 5 types of graphs:
 * - Scatter graph, X(Y);
 * - Polar graph, Ro(Phi);
 * - Parametric graph, X(t), Y(t);
 * - Integral graph, Yi+1 = Yi-1 + Yi;
 * - Color map, X is t, Y and Z is colored.
 */
struct GraphProperties {
    QString name; //! Name of graph.
    QString x_name; //! Name of measured value of X axis.
    QString y_name; //! Name of measured value of Y axis.
    QString z_name; //! For color maps and waterfalls.
    QString x_title; //! Title of X axis.
    QString y_title; //! Title of Y axis.
    QString z_title; //! Title of Z axis.
    QString x_unit; //! Measurement unit of X axis.
    QString y_unit; //! Measurement unit of Y axis.
    QString z_unit; //! Measurement unit of Z axis.
    QString x_phisical_quantity; //! Physical quantity of X axis.
    QString y_phisical_quantity; //! Physical quantity of Y axis.
    QString z_phisical_quantity; //! Physical quantity of Z axis.
    GraphDir x_dir; //! Direction of X axis.
    GraphDir y_dir; //! Direction of Y axis.
    unsigned int total_N; //! Total number of stored points.
    unsigned int last_N_limit; //! For SHOW_LAST_N update method.
    GraphUpdateMode update_mode; //! See \link GraphUpdateMode \endlink.
    GraphScaleType x_scale; //! Type of scale of X axis.
    GraphScaleType y_scale; //! Type of scale of Y axis.
    GraphScaleType z_scale; //! Type of scale of Z axis.
    QVector<int> channels; //! e.g. 1, 2, 3, 4
    QColor color; //! Color of graph's curve (except "Color maps").
    GraphType graphType; //! Type of graph.
    QCPColorGradient::GradientPreset colorScale; //! Gradient type of Color map (for Color maps only).
    QString indexName; //! Measured value name, which is an index for Color Maps.w
};

QString ColorToNameConverter(const QColor &color);
QColor nameToColorConverter(const QString &name);
GraphType nameToGraphTypeConverter(const QString &name);
QString graphTypeToNameConverter(const GraphType &type);
QMultiMap<QString, Graph::MeasuredValueDescription> loadConfigJSON(const QString &pathToJSON);
QCPColorGradient::GradientPreset nameToColorScaleTypeConverter(const QString &name);
QString colorScaleTypeToNameConverter(const QCPColorGradient::GradientPreset &type);

};

#endif // COMMON_H
