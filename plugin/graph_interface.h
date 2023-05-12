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

#ifndef GRAPH_INTERFACE_H
#define GRAPH_INTERFACE_H

#include <QObject>
#include <QtPlugin>
#include <QString>
#include <QVariant>

struct MeasuredValue {
    uint64_t timestamp; //!< [мс], временная метка, возможно одна для разных параметров (одна на пакет)
    QString name; //!< Уникальное имя измеряемого параметра (velocity, time, temperature, pressure)
    int SI_to_current_mult = 1; //!< Коэффициент преобразования в единицу СИ, по умолчанию 1. [current / SI]
    int SI_to_current_shift = 0; //!< Сдвиг при преобразовании в единицу СИ, по умолчанию 0. [current - SI]
    // double value; //!< Измеренное значение
    QVariant value; //!< Измеренное значение или вектор значений (QVariantList)
    int channel; //!< Номер канала устройства
    bool is_valid = true;
};

class QDockWidget;
class QMainWindow;
class QToolBar;

//! [0]
class GraphInterface
{
public:
    // GraphInterface(QObject *parent = nullptr) {}
    // GraphInterface(QMainWindow *parent = nullptr) {}

    virtual ~GraphInterface() = default;
    // virtual QObject* getObject() = 0;
    virtual void addData(const MeasuredValue &value) = 0;
    virtual QToolBar* toolBar() const = 0;
    virtual QList<QDockWidget*> dockWindows() const = 0;
    virtual void setMainWindow(QMainWindow*) = 0;
    // Call it after setMainWindow
    virtual bool loadJSONs(QStringList subdirsNames = {}) = 0;
    virtual bool saveGraphPluginGeometry(const QString &suffix = "") = 0;
    virtual QString aboutInfo() = 0;
    // Change size of packet at runtime
    virtual void setPacketSize(int size, const QString tableName = "") = 0;
    virtual int packetSize(const QString tableName = "") const = 0;
    virtual QStringList getValuesNames(const QString &tableName = "") const = 0;
    virtual void clearAll() = 0;
    virtual void setUpdateable(bool isUpdateable = true) = 0;

//signals:
    //void newDockWindow(QDockWidget*);
};


QT_BEGIN_NAMESPACE

#define GraphInterface_iid "org.qt-project.Qt.Examples.GraphInterface"

Q_DECLARE_INTERFACE(GraphInterface, GraphInterface_iid)
QT_END_NAMESPACE

//! [0]
#endif
