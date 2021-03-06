/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "expectedeventlist.h"
#include <QDebug>
#include <QCoreApplication>
#include <QAbstractEventDispatcher>
#include <QtTest/QtTest>

ExpectedEventList::ExpectedEventList(QObject *target)
    : QObject(target), eventCount(0)
{
    target->installEventFilter(this);
    debug = qgetenv("NATIVEDEBUG").toInt();
    if (debug > 0)
        qDebug() << "Debug level sat to:" << debug;
}

ExpectedEventList::~ExpectedEventList()
{
    qDeleteAll(eventList);
}

void ExpectedEventList::append(QEvent *e)
{
    eventList.append(e);
    ++eventCount;
}

void ExpectedEventList::timerEvent(QTimerEvent *)
{
    timer.stop();
    QAbstractEventDispatcher::instance()->interrupt();
}

bool ExpectedEventList::waitForAllEvents(int maxEventWaitTime)
{
    if (eventList.isEmpty())
        return true;

    int eventCount = eventList.size();
    timer.start(maxEventWaitTime, this);

    while (timer.isActive()) {
        QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);
        if (eventList.isEmpty())
            return true;

        if (eventCount < eventList.size()){
            eventCount = eventList.size();
            timer.start(maxEventWaitTime, this);
        }
    }

    int eventListNr = eventCount - eventList.size() + 1;
    qWarning() << "Stopped waiting for expected event nr" << eventListNr;
    return false;
}

void ExpectedEventList::compareMouseEvents(QEvent *received, QEvent *expected)
{
    QMouseEvent *e1 = static_cast<QMouseEvent *>(received);
    QMouseEvent *e2 = static_cast<QMouseEvent *>(expected);

    // Do a manual check first to be able to write more sensible
    // debug output if we know we're going to fail:
    if (e1->pos() == e2->pos()
            && (e1->globalPos() == e2->globalPos())
            && (e1->button() == e2->button())
            && (e1->buttons() == e2->buttons())
            && (e1->modifiers() == e2->modifiers())) {
        if (debug > 0)
            qDebug() << "  Received (OK):" << e1 << e1->globalPos();
        return; // equal
    }

    // INVARIANT: The two events are not equal. So we fail. Depending
    // on whether debug mode is no or not, we let QTest fail. Otherwise
    // we let the test continue for debugging puposes.
    int eventListNr = eventCount - eventList.size();
    if (debug == 0) {
        qWarning() << "Expected event" << eventListNr << "differs from received event:";
        QCOMPARE(e1->pos(), e2->pos());
        QCOMPARE(e1->globalPos(), e2->globalPos());
        QCOMPARE(e1->button(), e2->button());
        QCOMPARE(e1->buttons(), e2->buttons());
        QCOMPARE(e1->modifiers(), e2->modifiers());
    } else {
        qWarning() << "*** FAIL *** : Expected event" << eventListNr << "differs from received event:";
        qWarning() << "Received:" << e1 << e1->globalPos();
        qWarning() << "Expected:" << e2 << e2->globalPos();
    }
}

void ExpectedEventList::compareKeyEvents(QEvent *received, QEvent *expected)
{
    QKeyEvent *e1 = static_cast<QKeyEvent *>(received);
    QKeyEvent *e2 = static_cast<QKeyEvent *>(expected);

    // Do a manual check first to be able to write more sensible
    // debug output if we know we're going to fail:
    if (e1->key() == e2->key()
            && (e1->modifiers() == e2->modifiers())
            && (e1->count() == e2->count())
            && (e1->isAutoRepeat() == e2->isAutoRepeat())) {
        if (debug > 0)
            qDebug() << "  Received (OK):" << e1 << QKeySequence(e1->key()).toString(QKeySequence::NativeText);
        return; // equal
    }

    // INVARIANT: The two events are not equal. So we fail. Depending
    // on whether debug mode is no or not, we let QTest fail. Otherwise
    // we let the test continue for debugging puposes.
    int eventListNr = eventCount - eventList.size();
    if (debug == 0) {
        qWarning() << "Expected event" << eventListNr << "differs from received event:";
        QCOMPARE(e1->key(), e2->key());
        QCOMPARE(e1->modifiers(), e2->modifiers());
        QCOMPARE(e1->count(), e2->count());
        QCOMPARE(e1->isAutoRepeat(), e2->isAutoRepeat());
    } else {
        qWarning() << "*** FAIL *** : Expected event" << eventListNr << "differs from received event:";
        qWarning() << "Received:" << e1 << QKeySequence(e1->key()).toString(QKeySequence::NativeText);
        qWarning() << "Expected:" << e2 << QKeySequence(e2->key()).toString(QKeySequence::NativeText);
    }
}

bool ExpectedEventList::eventFilter(QObject *, QEvent *received)
{
    if (debug > 1)
        qDebug() << received;
    if (eventList.isEmpty())
        return false;

    bool eat = false;
    QEvent *expected = eventList.first();
    if (expected->type() == received->type()) {
        eventList.removeFirst();
        switch (received->type()) {
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseMove:
            case QEvent::MouseButtonDblClick:
            case QEvent::NonClientAreaMouseButtonPress:
            case QEvent::NonClientAreaMouseButtonRelease:
            case QEvent::NonClientAreaMouseButtonDblClick:
            case QEvent::NonClientAreaMouseMove: {
                compareMouseEvents(received, expected);
                eat = true;
                break;
            }
            case QEvent::KeyPress:
            case QEvent::KeyRelease: {
                compareKeyEvents(received, expected);
                eat = true;
                break;
            }
            case QEvent::Resize: {
                break;
            }
            case QEvent::WindowActivate: {
                break;
            }
            case QEvent::WindowDeactivate: {
                break;
            }
            default:
                break;
        }
        if (eventList.isEmpty())
            QAbstractEventDispatcher::instance()->interrupt();
    }

    return eat;
}

