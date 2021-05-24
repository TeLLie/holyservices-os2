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

#include "CalendarWidgetPrivate.h"
#include "CalendarModel.h"
#include "CalendarWidget.h"
#include "CalendarView.h"

CalendarWidgetPrivate::CalendarWidgetPrivate() 
{
    q_ptr = nullptr;
    m_model = nullptr;
    m_view = nullptr;
    m_selection = nullptr;
}

QDate CalendarWidgetPrivate::date() const
{
    return m_model->date();
}

void CalendarWidgetPrivate::setDate(const QDate &date)
{
    m_model->setDate(date);
    update();
}

void CalendarWidgetPrivate::showMonth(int year, int month)
{
    if (m_model->shownYear() == year && m_model->shownMonth() == month)
        return;
    m_model->showMonth(year, month);

    emit q_ptr->currentMonthChanged(year, month);

    m_view->internalUpdate();
    update();
}

void CalendarWidgetPrivate::update()
{
    QDate currentDate = date();
    int row, column;
    m_model->cellForVisibleDate(currentDate, row, column);
    QModelIndex idx;
    if (row != -1 && column != -1) {
        idx = m_model->index(row, column);
        m_selection->setCurrentIndex(idx, QItemSelectionModel::SelectCurrent);
    } else {
        m_selection->reset();
    }
}

void CalendarWidgetPrivate::slotChangeDate(const QDate &date, bool changeMonth)
{
    QDate oldDate = m_model->date();
    setDate(date);
    QDate newDate = m_model->date();
    if (changeMonth) {
        showMonth(newDate.year(), newDate.month());
    }
    if (oldDate != newDate) {
        emit q_ptr->selectionChanged(newDate);
    }
}

void CalendarWidgetPrivate::editingFinished()
{
    emit q_ptr->activated(date());
}

//////////////////////
