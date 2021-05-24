/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** Licensees holding a valid Qt License Agreement may use this file in
** accordance with the rights, responsibilities and obligations
** contained therein.  Please consult your licensing agreement or
** contact sales@trolltech.com if any conditions of this licensing
** agreement are not clear to you.
**
** Further information about Qt licensing is available at:
** http://www.trolltech.com/products/qt/licensing.html or by
** contacting info@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
// Slightly adopted as guaranteed by GPLv2

#include "CalendarView.h"

#include "CalendarModel.h"

#include <QHeaderView>
#include <QKeyEvent>

CalendarView::CalendarView(QWidget *parent)
    : QTableView(parent),
    m_validDateClicked(false),
    m_weekNumbersShown(true),
    m_dayNamesShown(true),
    m_readOnly(false)
{
    setShowGrid(false);
}

bool CalendarView::weekNumbersShown() const
{
    return m_weekNumbersShown;
}

void CalendarView::setWeekNumbersShown(bool show)
{
    if (m_weekNumbersShown == show)
        return;

    m_weekNumbersShown = show;
    verticalHeader()->setVisible(m_weekNumbersShown);
    updateGeometries();
}

bool CalendarView::dayNamesShown() const
{
    return m_dayNamesShown;
}

void CalendarView::setDayNamesShown(bool show)
{
    if (m_dayNamesShown == show)
        return;

    m_dayNamesShown = show;
    horizontalHeader()->setVisible(m_dayNamesShown);
    updateGeometries();
}

bool CalendarView::isReadOnly() const
{
    return m_readOnly;
}

void CalendarView::setReadOnly(bool enable)
{
    if (m_readOnly == enable)
        return;

    m_readOnly = enable;
}

QModelIndex CalendarView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    CalendarModel *calendarModel = ::qobject_cast<CalendarModel *>(model());
    if (!calendarModel)
        return QTableView::moveCursor(cursorAction, modifiers);

    if (m_readOnly)
        return currentIndex();

    QDate currentDate = calendarModel->date();
    switch (cursorAction) {
        case QAbstractItemView::MoveUp:
            currentDate = currentDate.addDays(-7);
            break;
        case QAbstractItemView::MoveDown:
            currentDate = currentDate.addDays(7);
            break;
        case QAbstractItemView::MoveLeft:
            currentDate = currentDate.addDays(-1);
            break;
        case QAbstractItemView::MoveRight:
            currentDate = currentDate.addDays(1);
            break;
        case QAbstractItemView::MoveHome:
            currentDate = QDate(currentDate.year(), currentDate.month(), 1);
            break;
        case QAbstractItemView::MoveEnd:
            currentDate = QDate(currentDate.year(), currentDate.month(), currentDate.daysInMonth());
            break;
        case QAbstractItemView::MovePageUp:
            currentDate = currentDate.addMonths(-1);
            break;
        case QAbstractItemView::MovePageDown:
            currentDate = currentDate.addMonths(1);
            break;
        case QAbstractItemView::MoveNext:
        case QAbstractItemView::MovePrevious:
            return currentIndex();
        default:
            break;
    }
    emit changeDate(currentDate, true);
    return currentIndex();
}

void CalendarView::keyPressEvent(QKeyEvent *event)
{
    if (!m_readOnly) {
        switch (event->key()) {
            case Qt::Key_Return:
            case Qt::Key_Enter:
            case Qt::Key_Space:
                emit editingFinished();
                break;
            default:
                break;
        }
    }
    QTableView::keyPressEvent(event);
}

bool CalendarView::event(QEvent *event)
{
    if (event->type() == QEvent::FontChange) {
        CalendarModel *calendarModel = ::qobject_cast<CalendarModel *>(model());
        if (calendarModel) {
            QFont f = font();
            f.setPointSize(f.pointSize() - 3);
            calendarModel->setCompletingDaysFont(f);
        }
    }
    return QTableView::event(event);
}

QDate CalendarView::handleMouseEvent(QMouseEvent *event)
{
    CalendarModel *calendarModel = ::qobject_cast<CalendarModel *>(model());
    if (!calendarModel)
        return QDate();

    QPoint pos = event->pos();
    QModelIndex index = indexAt(pos);
    QDate date = calendarModel->visibleDateForCell(index.row(), index.column());
    if (date.isValid() && date >= calendarModel->minimumDate()
            && date <= calendarModel->maximumDate()) {
        return date;
    }
    return QDate();
}

void CalendarView::mouseDoubleClickEvent(QMouseEvent *event)
{
    CalendarModel *calendarModel = ::qobject_cast<CalendarModel *>(model());
    if (!calendarModel) {
        QTableView::mouseDoubleClickEvent(event);
        return;
    }

    if (m_readOnly)
        return;

    QDate date = handleMouseEvent(event);
    m_validDateClicked = false;
    if (date == calendarModel->date()) {
        emit editingFinished();
    }
    /*
    if (date.isValid()) {
        m_validDateClicked = true;
        emit changeDate(date, false);
    } else {
        m_validDateClicked = false;
    }
    */
}

void CalendarView::mousePressEvent(QMouseEvent *event)
{
    CalendarModel *calendarModel = ::qobject_cast<CalendarModel *>(model());
    if (!calendarModel) {
        QTableView::mousePressEvent(event);
        return;
    }

    if (m_readOnly)
        return;

    QDate date = handleMouseEvent(event);
    if (date.isValid()) {
        m_validDateClicked = true;
        emit changeDate(date, false);
    } else {
        m_validDateClicked = false;
    }
}

void CalendarView::mouseMoveEvent(QMouseEvent *event)
{
    CalendarModel *calendarModel = ::qobject_cast<CalendarModel *>(model());
    if (!calendarModel) {
        QTableView::mouseMoveEvent(event);
        return;
    }

    if (m_readOnly)
        return;

    if (m_validDateClicked) {
        QDate date = handleMouseEvent(event);
        if (date.isValid()) {
            emit changeDate(date, false);
        }
    }
}

void CalendarView::mouseReleaseEvent(QMouseEvent *event)
{
    CalendarModel *calendarModel = ::qobject_cast<CalendarModel *>(model());
    if (!calendarModel) {
        QTableView::mouseReleaseEvent(event);
        return;
    }

    if (m_readOnly)
        return;

    if (m_validDateClicked) {
        QDate date = handleMouseEvent(event);
        if (date.isValid()) {
            emit changeDate(date, true);
        }
    }
}

