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

#ifndef CALNDAR_WIDGET_PRIVATE_H_INCLUDED_6D49CAFF_92BC_4485_9F76_FFD28E09C62C
#define CALNDAR_WIDGET_PRIVATE_H_INCLUDED_6D49CAFF_92BC_4485_9F76_FFD28E09C62C

#include <QDate>

class CalendarWidget;
class CalendarModel;
class CalendarView;

class QModelIndex;
class QItemSelectionModel;

class CalendarWidgetPrivate
{
    CalendarWidget *q_ptr;
    Q_DECLARE_PUBLIC(CalendarWidget)
public:
    CalendarWidgetPrivate();

    QDate date() const;
    void setDate(const QDate &date);

    void showMonth(int year, int month);

    void update();

    void slotCurrentChanged(const QModelIndex &current,
                    const QModelIndex &previous);
    void slotChangeDate(const QDate &date, bool changeMonth);
    void editingFinished();

    CalendarModel *m_model;
    CalendarView *m_view;
    QItemSelectionModel *m_selection;
};

#endif

