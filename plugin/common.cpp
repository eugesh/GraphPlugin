#include "common.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>

using namespace Graph;

/**
 * @brief nameToColorConverter
 * Color short name to Qt namespace colors types.
 * @param name supporeted names of colors: black, light gray, dark gray,
 * dark blue, dark red, dark green, dark Magenta, dark yellow, dark cyan,
 * blue, red, green, magenta, yellow, cyan.
 * @return
 */
QColor Graph::nameToColorConverter(const QString &name)
{
    if (name.contains("black", Qt::CaseInsensitive))
        return Qt::black;
    else if (name.contains("white", Qt::CaseInsensitive))
        return Qt::white;
    else if (name.contains("light", Qt::CaseInsensitive) &&
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

 QString Graph::ColorToNameConverter(const QColor &color)
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

GraphType Graph::nameToGraphTypeConverter(const QString &name)
{
    if (name.contains("scatter", Qt::CaseInsensitive))
        return GraphType::GraphScatter;
    else if (name.contains("polar", Qt::CaseInsensitive))
        return GraphType::GraphPolar;
    else if (name.contains("param", Qt::CaseInsensitive))
        return GraphType::GraphParametric;
    else if (name.contains("integr", Qt::CaseInsensitive))
        return GraphType::GraphIntegral;
    else if(name.contains("map", Qt::CaseInsensitive))
        return GraphType::GraphColorMap;

    // Default
    return GraphType::GraphScatter;
}

QString Graph::graphTypeToNameConverter(const GraphType &type)
{
    switch (type) {
    case GraphScatter:
        return QString("scatter");
    case GraphPolar:
        return QString("polar");
    case GraphParametric:
        return QString("parametric");
    case GraphIntegral:
        return QString("integral");
    case GraphColorMap:
        return QString("colorMap");
    default:
        return QString("scatter");
    }
}

/*
 * @brief The GradientPreset enum
 *                   gpGrayscale  ///< Continuous lightness from black to white (suited for non-biased data representation)
                    ,gpHot       ///< Continuous lightness from black over firey colors to white (suited for non-biased data representation)
                    ,gpCold      ///< Continuous lightness from black over icey colors to white (suited for non-biased data representation)
                    ,gpNight     ///< Continuous lightness from black over weak blueish colors to white (suited for non-biased data representation)
                    ,gpCandy     ///< Blue over pink to white
                    ,gpGeography ///< Colors suitable to represent different elevations on geographical maps
                    ,gpIon       ///< Half hue spectrum from black over purple to blue and finally green (creates banding illusion but allows more precise magnitude estimates)
                    ,gpThermal   ///< Colors suitable for thermal imaging, ranging from dark blue over purple to orange, yellow and white
                    ,gpPolar     ///< Colors suitable to emphasize polarity around the center, with blue for negative, black in the middle and red for positive values
                    ,gpSpectrum  ///< An approximation of the visible light spectrum (creates banding illusion but allows more precise magnitude estimates)
                    ,gpJet       ///< Hue variation similar to a spectrum, often used in numerical visualization (creates banding illusion but allows more precise magnitude estimates)
                    ,gpHues
 */
QCPColorGradient::GradientPreset Graph::nameToColorScaleTypeConverter(const QString &name)
{
    if (name.contains("grayscale", Qt::CaseInsensitive))
        return QCPColorGradient::gpGrayscale;
    else if (name.contains("hot", Qt::CaseInsensitive))
        return QCPColorGradient::gpHot;
    else if (name.contains("cold", Qt::CaseInsensitive))
        return QCPColorGradient::gpCold;
    else if(name.contains("night", Qt::CaseInsensitive))
        return QCPColorGradient::gpNight;
    else if(name.contains("candy", Qt::CaseInsensitive))
        return QCPColorGradient::gpCandy;
    else if (name.contains("geo", Qt::CaseInsensitive))
        return QCPColorGradient::gpGeography;
    else if (name.contains("ion", Qt::CaseInsensitive))
        return QCPColorGradient::gpIon;
    else if(name.contains("thermal", Qt::CaseInsensitive))
        return QCPColorGradient::gpThermal;
    else if(name.contains("polar", Qt::CaseInsensitive))
        return QCPColorGradient::gpPolar;
    else if (name.contains("spectr", Qt::CaseInsensitive))
        return QCPColorGradient::gpSpectrum;
    else if(name.contains("jet", Qt::CaseInsensitive))
        return QCPColorGradient::gpJet;
    else if(name.contains("hue", Qt::CaseInsensitive))
        return QCPColorGradient::gpHues;

    // Default
    return QCPColorGradient::gpJet;
}

QString Graph::colorScaleTypeToNameConverter(const QCPColorGradient::GradientPreset &type)
{
    switch (type) {
    case QCPColorGradient::gpGrayscale:
        return QString("grayscale");
    case QCPColorGradient::gpHot:
        return QString("hot");
    case QCPColorGradient::gpCold:
        return QString("cold");
    case QCPColorGradient::gpNight:
        return QString("night");
    case QCPColorGradient::gpCandy:
        return QString("candy");
    case QCPColorGradient::gpGeography:
        return QString("geography");
    case QCPColorGradient::gpIon:
        return QString("ion");
    case QCPColorGradient::gpThermal:
        return QString("thermal");
    case QCPColorGradient::gpPolar:
        return QString("polar");
    case QCPColorGradient::gpSpectrum:
        return QString("spectrum");
    case QCPColorGradient::gpJet:
        return QString("jet");
    case QCPColorGradient::gpHues:
        return QString("hues");

    default:
        return QString("jet");
    }
}

 /**
  * @brief loadConfigJSON
  * Reads JSON file with Measured Values descriptions.
  * @param pathToJSON path to JSON file (e.g. "plugin_config.json").
  * @return multimap "Measured Value name" -> "struct MeasuredValueDescription".
  */
 QMultiMap<QString, MeasuredValueDescription> Graph::loadConfigJSON(const QString &pathToJSON)
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
         // Time is always the first coloumn of history table.
         if (valueObject["name"].toString().contains("time", Qt::CaseInsensitive))
             continue; // Skip time, it is not "value to measure".

         MeasuredValueDescription mvdesc_struct;
         mvdesc_struct.name = valueObject["name"].toString();
         mvdesc_struct.desc = valueObject["description"].toString();
         mvdesc_struct.desc_ru = valueObject["description_ru"].toString();
         mvdesc_struct.physQuant = valueObject["physicalQuantity"].toString();
         mvdesc_struct.unit = valueObject["measure_unit"].toString();
         mvdesc_struct.unit_rus = valueObject["measure_unit_rus"].toString();
         mvdesc_struct.symbol = valueObject["symbol"].toString();
         mvdesc_struct.symbol_rus = valueObject["symbol_rus"].toString();
         mvdesc_struct.index = valueObject["index"].toInt();
         mvdesc_struct.type = valueObject["type"].toString();
         for (auto val : valueObject["values"].toArray())
            mvdesc_struct.possibleValues.push_back(val.toString());
         measValDescMap.insert(mvdesc_struct.name, mvdesc_struct);
     }

     return measValDescMap;
 }
