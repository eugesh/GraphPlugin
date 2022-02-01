#include "graphplugintablemodel.h"


GraphPluginTableModel::GraphPluginTableModel(const QStringList &titles, const QStringList &names, GraphPluginMode syncMode, QObject *parent) :
    QAbstractTableModel(parent),
    m_coloumnNames(names),
    m_coloumnTitles(titles),
    m_syncMode(syncMode)
{
    // ToDo: Must be read from JSON
    // m_coloumnTitles << "Timestamp" << "Pressure" << "Temperature" << "Sound Velocity";
    // m_coloumnTitles << tr("Время") << tr("Давление") << tr("Температура") << tr("Скорость звука");
    // m_coloumnNames << "time" << "pressure" << "temperature" << "velocity";
}

void GraphPluginTableModel::appendValue(const MeasuredValue &val)
{
    //m_dataMap[val.timestamp] = val;
    m_dataMap.insertMulti(val.timestamp, val);

    if (m_dataMap.values(val.timestamp).size() == m_packetSize && m_syncMode == GRAPH_DATA_SYNCH) {
        emit packetFormed(m_dataMap.values(val.timestamp)); // To Plot
        addRow(m_dataMap.values(val.timestamp));
        return;
    }

    // Unique timestamps
    if (!m_timeStamps.contains(val.timestamp))
        m_timeStamps.append(val.timestamp);
}

void GraphPluginTableModel::addRow(const QList<MeasuredValue> &packet)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    endInsertRows();
}

void GraphPluginTableModel::clear()
{

}

int GraphPluginTableModel::columnCount(const QModelIndex &parent) const
{
    return m_packetSize + 1; // + Timestamp
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
                return val.value; // ToDo * by SI convertion coefficient;
    }
    case ClipboardTextRole:
        return {}; // ToDo
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
