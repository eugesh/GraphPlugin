#include "addonplotdialog.h"
#include "common.h"
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

        QMenu contextMenu(tr("Context menu"), this);

        QAction action1(tr("Копировать"), this);
        QAction action2(tr("Вывести на график"), this);

        connect(&action1, &QAction::triggered, this, &GraphTableView::copyColumn);
        connect(&action2, &QAction::triggered, this, &GraphTableView::placeOnPlot);

        if (selectedIndexes().count())
            contextMenu.addAction(&action1);
        contextMenu.addAction(&action2);

        contextMenu.exec(mapToGlobal(pos));
    });
}

void GraphTableView::setConfig(GraphPluginConfig *config)
{
    m_config = config;

    m_addPointDialog = new AddOnPlotDialog;
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

    if (m_addPointDialog->exec()) {
        //GraphProperties prop;

        //prop = m_addPointDialog->getProp();

        emit createNewGraph(m_addPointDialog->getCustomPlotName(), m_addPointDialog->getProp());
    }
}
