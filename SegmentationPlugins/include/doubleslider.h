/****************************************************************************
 **
 ** Copyright (C) Qxt Foundation. Some rights reserved.
 **
 ** This file is part of the QxtGui module of the Qxt library.
 **
 ** This library is free software; you can redistribute it and/or modify it
 ** under the terms of the Common Public License, version 1.0, as published
 ** by IBM, and/or under the terms of the GNU Lesser General Public License,
 ** version 2.1, as published by the Free Software Foundation.
 **
 ** This file is provided "AS IS", without WARRANTIES OR CONDITIONS OF ANY
 ** KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT LIMITATION, ANY
 ** WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY OR
 ** FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** You should have received a copy of the CPL and the LGPL along with this
 ** file. See the LICENSE file and the cpl1.0.txt/lgpl-2.1.txt files
 ** included with the source distribution for more information.
 ** If you did not receive a copy of the licenses, contact the Qxt Foundation.
 **
 ** <http://libqxt.org>  <foundation@libqxt.org>
 **
 ****************************************************************************/
#ifndef DOUBLESLIDER_H
#define DOUBLESLIDER_H

#include <QDebug>
#include <QSlider>
#include <QStyle>
#include <QObject>
#include <QStylePainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QStyleOptionSlider>

class DoubleSlider : public QSlider
{
    Q_OBJECT
    Q_PROPERTY(int lowerValue READ lowerValue WRITE setLowerValue)
    Q_PROPERTY(int upperValue READ upperValue WRITE setUpperValue)

public:
    enum Handle {
        HandleNone,
        HandleLower,
        HandleUpper
    };

    Q_ENUMS(Handle)
    explicit DoubleSlider(QWidget* parent = 0);
//    virtual ~DoubleSlider();

    int lowerValue() const;
    int upperValue() const;
    QVector<int> getValues() const;
    int pixelPosToRangeValue(int pos);

public Q_SLOTS:
    void setLowerValue(int lower);
    void setUpperValue(int upper);
    void setSpan(int, int);
    void setRange(int, int);

Q_SIGNALS:
    void spanChanged(int lower, int upper);
    void lowerValueChanged(int lower);
    void upperValueChanged(int upper);
    void lowerPositionChanged(int lower);
    void upperPositionChanged(int upper);

protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void paintEvent(QPaintEvent* event);

private:
    DoubleSlider::Handle selectedHandle;
    DoubleSlider::Handle clickedHandle(QMouseEvent *event);
    int lower;
    int upper;
    QVector<int> value();

};

#endif // DOUBLESLIDER_H
