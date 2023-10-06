#include "graphpluginconfig.h"
#include "graphtablemodel.h"


GraphTableModel::GraphTableModel(const QStringList &titles, const QStringList &names, GraphPluginMode syncMode, QObject *parent) :
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

void GraphTableModel::setPacketSize(int n)
{
    m_packetSize = n;
    if (!m_timeStamps.isEmpty() && !m_dataMap.isEmpty())
        checkIsPackedFormed();
}

void GraphTableModel::checkIsPackedFormed()
{
    // Check if packet is already formed
    if (m_dataMap.values(m_timeStamps.last()).size() == m_packetSize && m_syncMode == GRAPH_DATA_SYNCH) {
        // addRow(m_dataMap.values(m_timeStamps.last()));
        emit packetFormed(m_dataMap.values(m_timeStamps.last())); // To Plot
        return;
    }
}

void GraphTableModel::appendValue(const MeasuredValue &val)
{
    // Add value to data map
    m_dataMap.insertMulti(val.timestamp, val);

    // Cleaning. Store lt m_ringBufferSize elements
    while (rowCount() > m_ringBufferSize) {
        beginRemoveRows({}, 0, rowCount() - m_ringBufferSize - 1);
        auto firstT = m_timeStamps.dequeue();
        while (m_dataMap.contains(firstT))
            m_dataMap.remove(firstT);
        endRemoveRows();
    }

    // Unique timestamps
    if (m_timeStamps.isEmpty() || m_timeStamps.last() != val.timestamp) // O(1)
        m_timeStamps.enqueue(val.timestamp);

    if (m_dataMap.values(val.timestamp).size() == m_packetSize && m_syncMode == GRAPH_DATA_SYNCH) {
        addRow(m_dataMap.values(val.timestamp));
        emit packetFormed(m_dataMap.values(val.timestamp)); // To Plot
        return;
    }
}

void GraphTableModel::addRow(const QList<MeasuredValue> &packet)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    endInsertRows();
}

void GraphTableModel::clear()
{
    beginResetModel();

    m_dataMap.clear();
    m_timeStamps.clear();

    endResetModel();
}

int GraphTableModel::columnCount(const QModelIndex &parent) const
{
    // return m_packetSize + 1; // + Timestamp
    // return m_coloumnNames.size();
    return parent.isValid() ? 0 : m_coloumnNames.size();
}

QVariant GraphTableModel::data(const QModelIndex &index, int role) const
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

QVariant GraphTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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

int GraphTableModel::rowCount(const QModelIndex &parent) const
{
    // return m_timeStamps.size();
    return parent.isValid() ? 0 : m_timeStamps.size();
}
