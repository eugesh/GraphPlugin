#include "addonplotdialog.h"
#include "graphmainwindow.h"
#include "graphpluginconfig.h"
#include "graphtableview.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMenu>


GraphTableView::GraphTableView(QWidget *parent)
 : QTableView(parent)
{
    setObjectName("GraphTableView");

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QWidget::customContextMenuRequested,
            this,
        [this] (const QPoint &pos) {
        QMenu contextMenu(tr("Context menu"), this);

        QAction action1(tr("Копировать"), this);
        // QAction action2("Select all", this);
        QAction action2(tr("Выделить все"), this);

        connect(&action1, &QAction::triggered, this, &GraphTableView::copyRow);
        connect(&action2, &QAction::triggered, this, &QAbstractItemView::selectAll);

        if (selectedIndexes().count())
            contextMenu.addAction(&action1);
        contextMenu.addAction(&action2);

        // contextMenu.exec(mapToGlobal(pos));
        auto size = contextMenu.sizeHint();
        contextMenu.setWindowModality(Qt::WindowModality::NonModal);
        contextMenu.popup(mapToGlobal(pos));
    }, Qt::QueuedConnection);

    // QHeaderView *header = new QHeaderView(Qt::Orientation::Horizontal, this);
    // header->setContextMenuPolicy(Qt::CustomContextMenu);
    horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(horizontalHeader(), &QWidget::customContextMenuRequested,
            this,
        [this] (const QPoint &pos) {
        m_currentColumn = horizontalHeader()->logicalIndexAt(pos);
        auto IndexList = selectedIndexes();

        QMenu contextMenu(tr("Context menu"), this);

        QAction action1(tr("Копировать"), this);
        QAction action2(tr("Вывести на график"), this);

        connect(&action1, &QAction::triggered, this, &GraphTableView::copyColumn);
        connect(&action2, &QAction::triggered, this, &GraphTableView::placeOnPlot);

        if (selectedIndexes().count())
            contextMenu.addAction(&action1);
        contextMenu.addAction(&action2);

        auto size = contextMenu.sizeHint();
        // contextMenu.exec(mapToGlobal(pos));
        contextMenu.setWindowModality(Qt::WindowModality::NonModal);
        contextMenu.popup(mapToGlobal(pos));
    }, Qt::QueuedConnection);
}

void GraphTableView::setConfig(GraphPluginConfig *config)
{
    m_config = config;

    m_addPointDialog = new AddOnPlotDialog(this);
    m_addPointDialog->setConfig(config);
}

void GraphTableView::setMeasValues(const QMap<QString, MeasuredValueDescription> &mvd)
{
    m_measValuesDesc = mvd;
    if (! m_addPointDialog) {
        qCritical() << "Error: setConfig first!";
        return;
    }
    m_addPointDialog->setMeasValDesc(mvd);
}

void GraphTableView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);

    /*for (int i = 0, count = model->columnCount(); i < count; i++) {
        const QSize size = model->headerData(i, Qt::Horizontal, Qt::SizeHintRole).value<QSize>();
        setColumnWidth(i, size.width());
    }*/
}

void GraphTableView::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Copy)) {
        copyRow();
    } else if (event->matches(QKeySequence::SelectAll)) {
        selectAll();
    } else {
        QTableView::keyPressEvent(event);
    }
}

void GraphTableView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu contextMenu(tr("Context menu"), this);

    QAction action1(tr("Копировать"), this);
    // QAction action2("Select all", this);
    QAction action2(tr("Выделить все"), this);

    connect(&action1, &QAction::triggered, this, &GraphTableView::copyRow);
    connect(&action2, &QAction::triggered, this, &QAbstractItemView::selectAll);

    if (selectedIndexes().count())
        contextMenu.addAction(&action1);
    contextMenu.addAction(&action2);

    // contextMenu.exec(mapToGlobal(pos));
    contextMenu.popup(mapToGlobal(event->globalPos()));
}

void GraphTableView::copyColumn()
{
    QClipboard *clipboard = QApplication::clipboard();

    const QModelIndexList ilist = selectedIndexes();

    QString strRow;

    for (const QModelIndex &index : ilist)
        strRow += index.data(ClipboardTextRole).toString() + '\n';

    clipboard->setText(strRow);
}

void GraphTableView::copyRow()
{
    QClipboard *clipboard = QApplication::clipboard();

    const QModelIndexList ilist = selectedIndexes();

    QString strRow;

    for (const QModelIndex &index : ilist) {
        strRow += index.data(ClipboardTextRole).toString() + " ";
        if (index.column() == model()->columnCount() - 1)
            strRow += '\n';
    }

    clipboard->setText(strRow);
}

void GraphTableView::placeOnPlot()
{
    //model()->sel
    // auto selected = horizontalHeader()->selectionModel()->currentIndex();
    auto yName = model()->headerData(m_currentColumn, Qt::Horizontal).toString();
    auto yPhysQuant = model()->headerData(m_currentColumn, Qt::Horizontal, DataRole).toString();
    yPhysQuant = model()->headerData(m_currentColumn, Qt::Horizontal, PhysQuantRole).toString();

    auto prop = graphPropertiesFromSelectedColoumn(yPhysQuant);

    m_addPointDialog->setProp(prop);

    if (m_addPointDialog->exec()) {
        //GraphProperties prop;

        //prop = m_addPointDialog->getProp();

        emit createNewGraph(m_addPointDialog->getCustomPlotName(), m_addPointDialog->getProp());
    }
}

GraphProperties GraphTableView::graphPropertiesFromSelectedColoumn(const QString &physicalQuantityName) const
{
    GraphProperties prop;

    prop.name = m_measValuesDesc[physicalQuantityName].desc_ru;
    prop.x_dir = GraphDir::RIGHT;
    prop.y_dir = GraphDir::UP;
    prop.color = Qt::red;
    prop.x_name = "time";
    prop.y_name = m_measValuesDesc[physicalQuantityName].name;
    prop.x_phisical_quantity = "time";
    prop.y_phisical_quantity = physicalQuantityName;
    prop.x_unit = "second";
    prop.y_unit = m_config->getProperty(physicalQuantityName, m_measValuesDesc[physicalQuantityName].unit, "name").toString();
    prop.x_title = tr("Время");
    prop.y_title = m_measValuesDesc[physicalQuantityName].desc_ru;
    prop.update_mode = SHOW_ALL;
    //prop.channels // ToDo
    prop.x_scale = LIN;
    prop.y_scale = LIN;
    prop.last_N_limit = 3000;
    prop.total_N = 300000;
    prop.color = Qt::red;

    return prop;
}
