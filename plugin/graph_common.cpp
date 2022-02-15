#include "common.h"
#include "graph_common.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>

bool saveJSON(const QString &pathToJSON, const QList<GraphProperties> &properties, const QString &windowTitle)
{
    QFile saveFile(pathToJSON);

    if (! saveFile.open(QIODevice::WriteOnly)) {
        qCritical() << "Output file " << pathToJSON << " wasn't opened on write";
        return false;
    }

    QJsonObject docObject;
    QJsonObject docPropObject;
    QJsonArray customPlotArray;
    QJsonObject graphObject;

    docPropObject["name"] = windowTitle;
    docObject["name"] = docPropObject;

    for (auto prop : properties) {
        graphObject["name"] = prop.name;
        graphObject["x_name"] = prop.x_name;
        graphObject["y_name"] = prop.y_name;
        graphObject["x_title"] = prop.x_title;
        graphObject["y_title"] = prop.y_title;
        graphObject["x_unit"] = prop.x_unit;
        graphObject["y_unit"] = prop.y_unit;
        graphObject["x_phisical_quantity"] = prop.x_phisical_quantity;
        graphObject["y_phisical_quantity"] = prop.y_phisical_quantity;
        graphObject["x_dir"] = prop.x_dir == LEFT ? "left" : "right";
        graphObject["y_dir"] = prop.y_dir == DOWN ? "down" : "up";
        graphObject["total_N"] = static_cast<int>(prop.total_N);
        graphObject["last_N_limit"] = static_cast<int>(prop.last_N_limit);
        graphObject["update_mode"] = prop.update_mode == SHOW_LAST_N ? "SHOW_LAST_N" : "SHOW_ALL";

        graphObject["x_scale"] = prop.x_scale == GraphScaleType::LOG ? "LOG" : "LIN";
        graphObject["y_scale"] = prop.y_scale == GraphScaleType::LOG ? "LOG" : "LIN";

        graphObject["color"] = ColorToNameConverter(prop.color);

        QJsonArray channelsArray;
        for (auto ch : prop.channels) {
            channelsArray.append(ch);
        }
        graphObject["channels"] = channelsArray;

        customPlotArray.append(graphObject);
    }

    docObject["plots"] = customPlotArray;

    QJsonDocument saveDoc(docObject);
    saveFile.write(saveDoc.toJson());

    return true;
}
