#include "graphpluginconfig.h"

#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <qlogging.h>


/**
key:value(s) with example.

"name": "fahrenheit",                    // QString, arbitrary
"plural": "nautical miles",              // QString, arbitrary
"otherNames": ["degree fahrenheit", "degrees fahrenheit"],
                                         // QStringList, arbitrary
"symbol": "°F",                          // QStringList, arbitrary
"type": "customary",                     // QStringList, arbitrary
"systems": ["englishUnits", "imperial", "usCustomary"],
                                         // QStringList, arbitrary
"multiplier": 1.8,                       // double, 1.0 if absent
"prefixes": ["milli", "micro", "nano"],  // QStringList, arbitrary
"offset": -459.67,                       // double, arbitrary
"notes": "1/60 of minute"                // QString, arbitrary
 */
// typedef QMap<QString, QVariant> MeasUnit;

GraphPluginConfig::GraphPluginConfig(const QString &pathToUnits, const QString &pathToPrefixes)
{
    readPrefixes(pathToPrefixes);
    readAuxUnits(pathToUnits);
}

QMap<QString, MeasUnit> GraphPluginConfig::measurementUnits() const
{
    return m_measUnits;
}

QList<QMap<QString, QVariant>> GraphPluginConfig::auxMeasUnits(const QString &physValName) const
{
    return m_measUnits.values(physValName);
}

QStringList GraphPluginConfig::physicalValuesNames() const
{
    return m_measUnits.uniqueKeys();
}

QMap<QString, SIPrefix> GraphPluginConfig::prefixes() const
{
    return m_prefixes;
}

bool GraphPluginConfig::readPrefixes(const QString &pathToJSON)
{
    QFile loadFile(pathToJSON);

    if (! loadFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Input file " << pathToJSON << " wasn't opened on read";
        return false;
    }

    QByteArray loadData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(loadData));

    auto loadObj = loadDoc.object();

    auto keys = loadObj.keys();

    for (auto key : keys) {
        SIPrefix prefix;
        auto obj = loadObj[key].toObject();
        if (obj.contains("symbol"))
            prefix.symbol = obj["symbol"].toString();

        if (obj.contains("multiplier")) {
            //QString val = obj["multiplier"].toString();
            //prefix.multiplier = obj["multiplier"].toString().toDouble();
            prefix.multiplier = obj["multiplier"].toDouble();
        }

        m_prefixes.insert(key, prefix);
    }

    return true;
}

bool GraphPluginConfig::readAuxUnits(const QString &pathToJSON)
{
    QFile loadFile(pathToJSON);

    if (! loadFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Input file " << pathToJSON << " wasn't opened on read";
        return false;
    }

    QByteArray loadData = loadFile.readAll();

    QJsonParseError error;

    QJsonDocument loadDoc(QJsonDocument::fromJson(loadData, &error));

    if (error.error != QJsonParseError::ParseError::NoError) {
        auto err_string = error.errorString();
        qCritical() << "Error" << err_string << "while JSON" << pathToJSON << "reading";
    }

    auto loadObj = loadDoc.object();

    auto physValTypes = loadObj.keys();

    for (auto physValName : physValTypes) {

        auto obj = loadObj[physValName].toObject();
        auto physValKeys = obj.keys();

        QJsonObject units;

        for (auto pvk : physValKeys) {
            if (pvk.contains("units"))
                units = obj["units"].toObject();
        }

        auto unitTypes = units.keys();
        for (auto type : unitTypes) {
            auto unitObj = units[type].toObject();
            MeasUnit unit;
            unit.insert("name", type);
            if (unitObj.contains("symbol"))
                unit.insert("symbol", unitObj["symbol"].toString());
            if (unitObj.contains("multiplier"))
                unit.insert("multiplier", unitObj["multiplier"].toDouble());
            m_measUnits.insertMulti(physValName, unit);
        }
    }

    return true;
}
