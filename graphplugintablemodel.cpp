#include "graphplugintablemodel.h"


GraphPluginTableModel::GraphPluginTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    // Must be read from JSON
    m_coloumnNames << "Timestamp" << "Pressure" << "Temperature" << "Sound Velocity";

}

void GraphPluginTableModel::appendValue(const MeasuredValue &val)
{
    m_dataMap[val.timestamp] = val;

    if (m_dataMap.values(val.timestamp).size() == m_packetSize) {
        emit packetFormed(m_dataMap.values(val.timestamp));
        addRow(m_dataMap.values(val.timestamp));
        return;
    }

    if (!m_timeStamps.contains(val.timestamp))
        m_timeStamps.append(val.timestamp);
}

void GraphPluginTableModel::addRow(const QList<MeasuredValue> &packet)
{

}

void GraphPluginTableModel::clear()
{

}

int GraphPluginTableModel::columnCount(const QModelIndex &parent) const
{
    return m_packetSize;
}

QVariant GraphPluginTableModel::data(const QModelIndex &index, int role) const
{
    int col = index.column();

    auto name = m_coloumnNames[col];

    switch (role) {
    case Qt::TextAlignmentRole:
        return {};
    case Qt::DisplayRole: {
        for(MeasuredValue val : m_dataMap.values(m_timeStamps[col]))
            if (val.name == name)
                return val.value; // ToDo * by SI convertion coefficient;
    }
    //case ClipboardTextRole:
    default:
        return {};
    }
}

QVariant GraphPluginTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
        return m_coloumnNames[section];
    }

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
