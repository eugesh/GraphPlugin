#ifndef GRAPHPLUGINCONFIG_H
#define GRAPHPLUGINCONFIG_H

#include <QMap>
#include <QString>
#include <QVariant>

struct SIPrefix {
    QString name;
    QString name_ru;
    QString symbol;
    QString symbol_ru;
    double multiplier;
};

/*struct MeasUnit {
    QString name;

};*/

typedef QMap<QString, QVariant> MeasUnit;

class GraphPluginConfig
{
public:
    GraphPluginConfig(const QString &pathToUnits, const QString &pathToPrefixes);
    QMap<QString, MeasUnit> measurementUnits() const;
    QList<QMap<QString, QVariant>> auxMeasUnits(const QString &physValName) const;
    QStringList physicalValuesNames() const;
    QMap<QString, SIPrefix> prefixes() const;
    QVariant getProperty(const QString &key) const;

protected:
    bool readPrefixes(const QString &filepath);
    bool readAuxUnits(const QString &filepath);

private:
    //!< Prefix name, struct
    QMap<QString, SIPrefix> m_prefixes;
    //!< Physical quantity name, MeasUnit properties, one multiple
    QMap<QString, MeasUnit> m_measUnits;
};

#endif // GRAPHPLUGINCONFIG_H