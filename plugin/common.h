#ifndef COMMON_H
#define COMMON_H

#include <qnamespace.h>
#include <QColor>

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

QString ColorToNameConverter(const QColor &color);
QColor nameToColorConverter(const QString &name);
QMap<QString, MeasuredValueDescription> loadConfigJSON(const QString &pathToJSON);

#endif // COMMON_H
