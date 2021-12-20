#include "common.h"

QColor nameToColorConverter(const QString &name) {
    if (name.contains("black", Qt::CaseInsensitive))
        return Qt::black;
    else if (name.contains("white", Qt::CaseInsensitive))
        return Qt::white;
    else if (name.contains("lght", Qt::CaseInsensitive) &&
             name.contains("gray", Qt::CaseInsensitive))
        return Qt::lightGray;
    else if (name.contains("dark", Qt::CaseInsensitive) &&
             name.contains("gray", Qt::CaseInsensitive))
        return Qt::darkGray;
    else if (name.contains("dark", Qt::CaseInsensitive) &&
             name.contains("red", Qt::CaseInsensitive))
        return Qt::darkRed;
    else if (name.contains("dark", Qt::CaseInsensitive) &&
             name.contains("blue", Qt::CaseInsensitive))
        return Qt::darkBlue;
    else if (name.contains("dark", Qt::CaseInsensitive) &&
             name.contains("green", Qt::CaseInsensitive))
        return Qt::darkGreen;
    else if (name.contains("dark", Qt::CaseInsensitive) &&
             name.contains("Cyan", Qt::CaseInsensitive))
        return Qt::darkCyan;
    else if (name.contains("dark", Qt::CaseInsensitive) &&
             name.contains("Magenta", Qt::CaseInsensitive))
        return Qt::darkMagenta;
    else if (name.contains("dark", Qt::CaseInsensitive) &&
             name.contains("yellow", Qt::CaseInsensitive))
        return Qt::darkYellow;

    else if (name.contains("red", Qt::CaseInsensitive))
        return Qt::red;
    else if (name.contains("blue", Qt::CaseInsensitive))
        return Qt::blue;
    else if (name.contains("green", Qt::CaseInsensitive))
        return Qt::green;
    else if (name.contains("Cyan", Qt::CaseInsensitive))
        return Qt::cyan;
    else if (name.contains("Magenta", Qt::CaseInsensitive))
        return Qt::magenta;
    else if (name.contains("yellow", Qt::CaseInsensitive))
        return Qt::yellow;

    return QColor(rand() % 245 + 10, rand() % 245 + 10, rand() % 245 + 10);
}

 QString ColorToNameConverter(const QColor &color) {
    switch (color.rgb()) {
    case Qt::black:
        return QString("black");
    case Qt::white:
        return QString("white");
    case Qt::lightGray:
        return QString("lightGray");
    case Qt::darkGray:
        return QString("darkGray");
    case Qt::darkRed:
        return QString("darkRed");
    case Qt::darkBlue:
        return QString("darkBlue");
    case Qt::darkGreen:
        return QString("darkGreen");
    case Qt::darkCyan:
        return QString("darkCyan");
    case Qt::darkMagenta:
        return QString("darkMagenta");
    case Qt::darkYellow:
        return QString("darkYellow");
    case Qt::red:
        return QString("red");
    case Qt::blue:
        return QString("blue");
    case Qt::green:
        return QString("green");
    case Qt::cyan:
        return QString("cyan");
    case Qt::magenta:
        return QString("magenta");
    case Qt::yellow:
        return QString("yellow");
    default:
        return color.name();
    }
}
