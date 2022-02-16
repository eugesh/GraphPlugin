#include "graphpluginconfig.h"

#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
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

QMultiMap<QString, MeasUnit> GraphPluginConfig::measurementUnits() const
{
    return m_measUnits;
}

QList<MeasUnit> GraphPluginConfig::measurementUnits(const QString &name) const
{
    return m_measUnits.values(name);
}

QList<QMultiMap<QString, QVariant>> GraphPluginConfig::auxMeasUnits(const QString &physValName) const
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

    // Prefixes names
    auto keys = loadObj.keys();

    for (auto key : keys) {
        SIPrefix prefix;
        prefix.name = key;
        auto obj = loadObj[key].toObject();
        if (obj.contains("symbol"))
            prefix.symbol = obj["symbol"].toString();

        if (obj.contains("multiplier")) {
            //QString val = obj["multiplier"].toString();
            //prefix.multiplier = obj["multiplier"].toString().toDouble();
            prefix.multiplier = obj["multiplier"].toDouble();
        }

        if (obj.contains("name_ru"))
            prefix.name_ru = obj["name_ru"].toString();

        if (obj.contains("symbol_ru"))
            prefix.symbol_ru = obj["symbol_ru"].toString();

        m_prefixes.insert(key, prefix);
    }

    return true;
}

/**
 * @brief GraphPluginConfig::getProperty
 * @param physQuantName name of physical quantity (time, velocity, pressure, temperature, length, etc)
 * @param key "name", "name_ru", "symbol", "symbol_ru", "multiplier", "offset", "prefixes"
 * @return
 */
QVariant GraphPluginConfig::getProperty(const QString &physQuantName, const QString &unit, const QString &key) const
{
    for (auto val : m_measUnits.values(physQuantName)) {
        if (val.values("name").first().toString() == unit)
           return  val.values(key);
    }
    return {};
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
            unit.insert("name_ru", unitObj["name_ru"].toString());
            if (unitObj.contains("symbol"))
                unit.insert("symbol", unitObj["symbol"].toString());

            if (unitObj.contains("symbol_ru"))
                unit.insert("symbol_ru", unitObj["symbol_ru"].toString());
            else
                unit.insert("symbol_ru", unitObj["symbol"].toString());

            if (unitObj.contains("multiplier"))
                unit.insert("multiplier", unitObj["multiplier"].toDouble());
            else
                unit.insert("multiplier", 1.0);

            if (unitObj.contains("offset"))
                unit.insert("offset", unitObj["offset"].toDouble());
            else
                unit.insert("offset", 0.0);

            if (unitObj.contains("prefixes")) {
                QJsonArray prefixesArray = unitObj["prefixes"].toArray();
                for (int i = 0; i < prefixesArray.size(); ++i) {
                    // QJsonObject prefixObject = prefixesArray[i].toObject();
                    unit.insert("prefixes", prefixesArray[i].toString());
                }
            }

            m_measUnits.insert(physValName, unit);
        }
    }

    return true;
}

QMap<QString, double> GraphPluginConfig::getMultipliers(const QString &physQuantityName) const
{
    QMap<QString, double> map;

    auto unitsMap = m_measUnits.values(physQuantityName);

    for (auto unitMap : unitsMap) {
        map.insert(unitMap.values("symbol_ru").first().toString(),
                   unitMap.values("multiplier").first().toDouble());
    }

    return map;
}

QMap<QString, double> GraphPluginConfig::getMultipliersWithPrefixes(const QString &physQuantityName) const
{
    QMap<QString, double> map;

    auto unitsMap = m_measUnits.values(physQuantityName);
    for (auto unitMap : unitsMap) {
        map.insert(unitMap.values("symbol_ru").first().toString(),
                   unitMap.values("multiplier").first().toDouble());
        for (auto prefix : unitMap.values("prefixes")) {
            QString prefNameRu = m_prefixes[prefix.toString()].symbol_ru;
            double mult = m_prefixes[prefix.toString()].multiplier;

            map.insert(prefNameRu + unitMap.values("symbol_ru").first().toString(),
                       unitMap.values("multiplier").first().toDouble() * mult);
        }
    }

    return map;
}

QMap<QString, double> GraphPluginConfig::getOffsets(const QString &physQuantityName) const
{
    QMap<QString, double> map;

    auto unitsMap = m_measUnits.values(physQuantityName);

    for (auto unitMap : unitsMap) {
        map.insert(unitMap.values("symbol_ru").first().toString(),
                   unitMap.values("offset").first().toDouble());
    }

    return map;
}
