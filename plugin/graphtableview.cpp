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

        contextMenu.exec(mapToGlobal(pos));
    });

    // QHeaderView *header = new QHeaderView(Qt::Orientation::Horizontal, this);
    // header->setContextMenuPolicy(Qt::CustomContextMenu);
    horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(horizontalHeader(), &QWidget::customContextMenuRequested,
        [this] (const QPoint &pos) {
        m_currentColumn = horizontalHeader()->logicalIndexAt(pos);
        auto IndexList = selectedIndexes();

        QMenu contextMenu(tr("Context menu"), this);

        QAction action1(tr("Копировать"), this);
        QAction action2(tr("Вывести на график"), this);
        QAction action3(tr("Вывести на индикатор"), this);

        connect(&action1, &QAction::triggered, this, &GraphTableView::copyColumn);
        connect(&action2, &QAction::triggered, this, &GraphTableView::placeOnPlot);
        connect(&action3, &QAction::triggered, this, &GraphTableView::placeOnVectorIndicator);

        if (selectedIndexes().count())
            contextMenu.addAction(&action1);
        contextMenu.addAction(&action2);
        contextMenu.addAction(&action3);

        contextMenu.exec(mapToGlobal(pos));
    });
}

void GraphTableView::setConfig(GraphPluginConfig *config)
{
    m_config = config;

    m_addPlotDialog = new AddOnPlotDialog(this);
    m_addPlotDialog->setConfig(config);
}

void GraphTableView::setMeasValues(const QMultiMap<QString, MeasuredValueDescription> &mvd)
{
    m_measValuesDesc = mvd;
    if (! m_addPlotDialog) {
        qCritical() << "Error: setConfig first!";
        return;
    }
    m_addPlotDialog->setMeasValDesc(mvd);
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

    m_addPlotDialog->setProp(prop);
    m_addPlotDialog->setTitle(tr("Добавить на график"));

    if (m_addPlotDialog->exec()) {
        //GraphProperties prop;

        //prop = m_addPointDialog->getProp();

        emit createNewGraph(m_addPlotDialog->getCustomPlotName(), m_addPlotDialog->getProp());
    }
}

void GraphTableView::placeOnVectorIndicator()
{
    auto yName = model()->headerData(m_currentColumn, Qt::Horizontal).toString();
    auto yPhysQuant = model()->headerData(m_currentColumn, Qt::Horizontal, DataRole).toString();
    yPhysQuant = model()->headerData(m_currentColumn, Qt::Horizontal, PhysQuantRole).toString();

    auto prop = graphPropertiesFromSelectedColoumn(yPhysQuant);

    m_addPlotDialog->setProp(prop);
    m_addPlotDialog->setTitle(tr("Добавить на стрелочный индикатор"));

    if (m_addPlotDialog->exec()) {
        emit createNewVectorIndicator(m_addPlotDialog->getCustomPlotName(), m_addPlotDialog->getProp());
    }
}

GraphProperties GraphTableView::graphPropertiesFromSelectedColoumn(const QString &physicalQuantityName) const
{
    GraphProperties prop;

    prop.name = m_measValuesDesc.value(physicalQuantityName).desc_ru;
    prop.x_dir = GraphDir::RIGHT;
    prop.y_dir = GraphDir::UP;
    prop.color = Qt::red;
    prop.x_name = "time";
    prop.y_name = m_measValuesDesc.value(physicalQuantityName).name;
    prop.x_phisical_quantity = "time";
    prop.y_phisical_quantity = physicalQuantityName;
    prop.x_unit = "second";
    prop.y_unit = m_config->getProperty(physicalQuantityName, m_measValuesDesc.value(physicalQuantityName).unit, "name").toString();
    prop.x_title = tr("Время");
    prop.y_title = m_measValuesDesc.value(physicalQuantityName).desc_ru;
    prop.update_mode = SHOW_ALL;
    //prop.channels // ToDo
    prop.x_scale = LIN;
    prop.y_scale = LIN;
    prop.last_N_limit = 3000;
    prop.total_N = 300000;
    prop.color = Qt::red;

    return prop;
}
