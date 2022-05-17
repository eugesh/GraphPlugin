#ifndef GRAPHTOOLBAR_H
#define GRAPHTOOLBAR_H

#include <QObject>
#include <QToolBar>

class GraphToolbar : public QToolBar
{
    Q_OBJECT
public:
    GraphToolbar();

    bool initFromJSON(const QString &path);
};

#endif // GRAPHTOOLBAR_H
