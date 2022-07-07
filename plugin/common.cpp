#include "common.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>

QColor nameToColorConverter(const QString &name)
{
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

 QString ColorToNameConverter(const QColor &color)
 {
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

 QMultiMap<QString, MeasuredValueDescription> loadConfigJSON(const QString &pathToJSON)
 {
     QFile loadFile(pathToJSON);

     if (!loadFile.open(QIODevice::ReadOnly)) {
         qCritical() << "Input file " << pathToJSON << " wasn't opened on read";
         return {};
     }

     QByteArray loadData = loadFile.readAll();

     QMultiMap<QString, MeasuredValueDescription> measValDescMap;

     QJsonDocument loadDoc(QJsonDocument::fromJson(loadData));

     QJsonArray valuesArray = loadDoc.object()["values"].toArray();

     for (int i = 0; i < valuesArray.size(); ++i) {
         QJsonObject valueObject = valuesArray[i].toObject();
         if (valueObject["name"].toString().contains("time", Qt::CaseInsensitive))
             continue;

         MeasuredValueDescription mvdesc_struct;
         mvdesc_struct.name = valueObject["name"].toString();
         mvdesc_struct.desc = valueObject["description"].toString();
         mvdesc_struct.desc_ru = valueObject["description_ru"].toString();
         mvdesc_struct.physQuant = valueObject["physicalQuantity"].toString();
         mvdesc_struct.unit = valueObject["measure_unit"].toString();
         mvdesc_struct.unit_rus = valueObject["measure_unit_rus"].toString();
         mvdesc_struct.symbol = valueObject["symbol"].toString();
         mvdesc_struct.symbol_rus = valueObject["symbol_rus"].toString();
         measValDescMap.insert(mvdesc_struct.name, mvdesc_struct);
     }

     return measValDescMap;
 }
