#include "graphpluginconfig.h"
#include "graphplugintablemodel.h"


GraphPluginTableModel::GraphPluginTableModel(const QStringList &titles, const QStringList &names, GraphPluginMode syncMode, QObject *parent) :
    QAbstractTableModel(parent),
    m_coloumnNames(QStringList() << "time" << names),
    m_coloumnTitles(QStringList() << tr("Время") << titles),
    m_syncMode(syncMode)
{
    // ToDo: Must be read from JSON
    // m_coloumnTitles << "Timestamp" << "Pressure" << "Temperature" << "Sound Velocity";
    // m_coloumnTitles << tr("Время") << tr("Давление") << tr("Температура") << tr("Скорость звука");
    // m_coloumnNames << "time" << "pressure" << "temperature" << "velocity";
}

void GraphPluginTableModel::appendValue(const MeasuredValue &val)
{
    m_dataMap.insertMulti(val.timestamp, val);

    while (rowCount() > m_ringBufferSize) {
        beginRemoveRows({}, 0, rowCount() - m_ringBufferSize - 1);
        auto firstT = m_timeStamps.dequeue();
        while (m_dataMap.contains(firstT))
            m_dataMap.remove(firstT);
        endRemoveRows();
    }

    // Unique timestamps
    // if (!m_timeStamps.contains(val.timestamp)) // Correct but this is O(N)
    if (m_timeStamps.isEmpty() || m_timeStamps.last() != val.timestamp) // O(1)
        m_timeStamps.enqueue(val.timestamp);

    if (m_dataMap.values(val.timestamp).size() == m_packetSize && m_syncMode == GRAPH_DATA_SYNCH) {
        addRow(m_dataMap.values(val.timestamp));
        emit packetFormed(m_dataMap.values(val.timestamp)); // To Plot
        return;
    }

}

void GraphPluginTableModel::addRow(const QList<MeasuredValue> &packet)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    endInsertRows();
}

void GraphPluginTableModel::clear()
{
    beginResetModel();

    m_dataMap.clear();
    m_timeStamps.clear();

    endResetModel();
}

int GraphPluginTableModel::columnCount(const QModelIndex &parent) const
{
    // return m_packetSize + 1; // + Timestamp
    return m_coloumnNames.size();
}

QVariant GraphPluginTableModel::data(const QModelIndex &index, int role) const
{
    int col = index.column();
    int row = index.row();

    auto name = m_coloumnNames[col];

    switch (role) {
    case Qt::TextAlignmentRole:
        return {};
    case Qt::DisplayRole: {
        if (col == 0) { // Timestamp column
            return QDateTime::fromMSecsSinceEpoch(m_timeStamps[row]);
        }
        for (MeasuredValue val : m_dataMap.values(m_timeStamps[row]))
            if (val.name == name)
                if (val.is_valid) {
                    if (val.value.type() == QVariant::List)
                        return val.value.toList().first();
                    else
                        return val.value; // ToDo * by SI conversion coefficient;
                } else {
                    return "-";
                }
    }
    case ClipboardTextRole:
        return {}; // ToDo
    case Qt::ToolTipRole:
        if (col == 0) { // Timestamp column
            return QDateTime::fromMSecsSinceEpoch(m_timeStamps[row]);
        }
        for (MeasuredValue val : m_dataMap.values(m_timeStamps[row]))
            if (val.name == name)
                if (val.is_valid) {
                    if (val.value.type() == QVariant::List) {
                        auto valuesList = val.value.toList();
                        QString output;
                        for (auto element : valuesList) {
                            output += QString("%1\n").arg(element.toDouble());
                        }
                        return output;
                    }
                } else {
                    return "-";
                }
    default:
        return {};
    }
}

QVariant GraphPluginTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section < 0 || section >= m_coloumnTitles.size())
        return {};

    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
        return m_coloumnTitles[section];

    if ((role == DataRole) && (orientation == Qt::Horizontal))
        return m_coloumnNames[section];

    if ((role == PhysQuantRole) && (orientation == Qt::Horizontal))
        return m_coloumnNames[section];

    /*if ((role == Qt::SizeHintRole) && (orientation == Qt::Horizontal)) {
        switch (section) {
        case Number:
            return QSize(80, 25);
        case Timestamp:
            return QSize(130, 25);
        case Flags:
            return QSize(25, 25);
        case CanID:
            return QSize(50, 25);
        case DLC:
            return QSize(25, 25);
        case Data:
            return QSize(200, 25);
        }
    }*/

    return {};
}

int GraphPluginTableModel::rowCount(const QModelIndex &parent) const
{
    return m_timeStamps.size();
}
