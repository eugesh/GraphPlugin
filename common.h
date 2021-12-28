#ifndef COMMON_H
#define COMMON_H

#include <qnamespace.h>
#include <QColor>

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


#endif // COMMON_H
