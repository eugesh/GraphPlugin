/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ECHOPLUGIN_H
#define ECHOPLUGIN_H

#include "graph_interface.h"

#include <QMainWindow>
#include <QMap>
#include <QObject>
#include <QtPlugin>

class GraphMainWindow;
class GraphPluginTableModel;
struct MeasuredValueDescrition;
class QTableView;

class GraphPlugin : public QObject, GraphInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.Examples.GraphInterface" FILE "graph_plugin.json")
    Q_INTERFACES(GraphInterface)

public:
    //!< May be created from QMainWindow only
    explicit GraphPlugin(QMainWindow *mw = nullptr);
    ~GraphPlugin() override;

    QString echo(const QString &message) override;
    //!<
    virtual void addData(const MeasuredValue &value) override;
    //!<
    virtual QToolBar *toolBar() const override;
    //!< All QDockWidgets which must be added to superior MainWindow
    virtual QList<QDockWidget*> dockWindows() const override;
    //!<
    virtual void setMainWindow(QMainWindow *mw) override;

    bool loadJSONs();

public slots:

private:
    // Values types, measurement units
    bool loadValuesJSON(const QString &pathToJSON);
    bool loadConfig(const QString &pathToJSON);
    // Common SI measuring units
    bool loadSI(const QString &pathToJSON);
    // Read each JSON for graph plot window
    bool loadGraphJSON(const QString &pathToJSON);
    bool loadTableJSON(const QString &pathToJSON);

private:
    // Pointer to superior MainWindow
    QMainWindow *m_mainWindow;
    // Pointers to DockWindows and Toolbar have to be added to superior MainWindow
    // Graphs
    QList<QDockWidget*> m_graphsDocks;
    QList<GraphMainWindow*> m_graphsMainWins;
    // Score Board
    QDockWidget *m_scoreBoardDock;
    // ToolBar
    QToolBar *m_toolbar;
    // Table
    QDockWidget *m_tableDock;
    GraphPluginTableModel *m_tableModel;
    QTableView *m_tableView;
    // QList<GraphMainWindow*> m_graphMainWins;

    // Data Dispatcher
    //!< Timestamp -> value, one key - multple values
    // Data config
    int m_ringBufferSize;
    QList<MeasuredValueDescrition> m_mvdesc_struct;
    //!< Unique Names of values
    QList<QString> m_valueNames;
    //!< Values descriptions
    QList<QString> m_valueDescs;

    // Received Data
    // QMap<int64_t, QString> m_dataMap;
    QList<int64_t> m_timeStamps;
};


//signals:
//  void newDockWindow(QDockWidget*);
// virtual QObject* getObject() override { return this; }

#endif
