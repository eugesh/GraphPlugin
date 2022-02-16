#ifndef GRAPHPLUGINTABLEMODEL_H
#define GRAPHPLUGINTABLEMODEL_H

#include <QAbstractItemModel>
#include <QObject>
#include <QStringList>
#include <qcustomplot.h>

#include "common.h"
#include "graph_interface.h"

class GraphPluginTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    // GraphPluginTableModel(GraphPluginMode syncMode = GRAPH_DATA_SYNCH, QObject *parent = nullptr);
    explicit GraphPluginTableModel(const QStringList &titles, const QStringList &names, GraphPluginMode syncMode = GRAPH_DATA_SYNCH, QObject *parent = nullptr);

    void setPacketSize(int n) { m_packetSize = n; }
    int packetSize() const { return m_packetSize; }

    void clear();

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

public slots:
    void appendValue(const MeasuredValue &val);

signals:
    // Signal emitted when packet is formed
    void packetFormed(const QList<MeasuredValue> &val);

private:
    void addRow(const QList<MeasuredValue> &packet);
    bool readJSON(const QString &path);

private:
    int m_packetSize = 3; // 1 (t) + number of values
    // Data Dispatcher
    //!< Timestamp -> value, one key - multple values
    int m_ringBufferSize;
    QMultiMap<uint64_t, MeasuredValue> m_dataMap;
    QList<uint64_t> m_timeStamps;
    QStringList m_coloumnNames;
    QStringList m_coloumnTitles;
    GraphPluginMode m_syncMode = GRAPH_DATA_SYNCH;
};

#endif // GRAPHPLUGINTABLEMODEL_H
