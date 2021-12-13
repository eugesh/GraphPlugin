#ifndef GRAPHPLUGINTABLEMODEL_H
#define GRAPHPLUGINTABLEMODEL_H

#include <qcustomplot.h>

#include <QAbstractItemModel>
#include <QObject>

class GraphPluginTableModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    GraphPluginTableModel(QObject *parent = nullptr);
};

#endif // GRAPHPLUGINTABLEMODEL_H
