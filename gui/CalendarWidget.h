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

#ifndef CCalendarWIDGET_H
#define CCalendarWIDGET_H

#include <QWidget>
#include <QDate>
#include "CalendarModel.h"
#include "CalendarWidgetPrivate.h"

class CalendarWidget : public QWidget
{
    Q_OBJECT
    Q_ENUMS(Qt::DayOfWeek)
    Q_PROPERTY(QDate selectedDate READ selectedDate WRITE selectDate)
    Q_PROPERTY(QDate minimumDate READ minimumDate WRITE setMinimumDate)
    Q_PROPERTY(QDate maximumDate READ maximumDate WRITE setMaximumDate)
    Q_PROPERTY(Qt::DayOfWeek firstDayOfWeek READ firstDayOfWeek WRITE setFirstDayOfWeek)
    Q_PROPERTY(bool completingDaysShown READ completingDaysShown WRITE setCompletingDaysShown)
    Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid)
    Q_PROPERTY(bool selectable READ isSelectable WRITE setSelectable)
    Q_PROPERTY(bool todayMarked READ isTodayMarked WRITE setTodayMarked)
    // below consider replacing with an enum
    Q_PROPERTY(bool showSundaysInRed READ showSundaysInRed WRITE setShowSundaysInRed)

public:
    CalendarWidget(QWidget *parent = nullptr);
    ~CalendarWidget();

    QDate selectedDate() const;
    int currentYear() const;
    int currentMonth() const;

    QDate minimumDate() const;
    void setMinimumDate(const QDate &date);

    QDate maximumDate() const;
    void setMaximumDate(const QDate &date);

    Qt::DayOfWeek firstDayOfWeek() const;
    void setFirstDayOfWeek(Qt::DayOfWeek dayOfWeek);

    bool completingDaysShown() const;
    void setCompletingDaysShown(bool show);

    bool showGrid() const;
    void setShowGrid(bool show);

    bool isSelectable() const;
    void setSelectable(bool enable);

    bool isTodayMarked() const;
    void setTodayMarked(bool marked);

    bool showSundaysInRed() const;
    void setShowSundaysInRed(bool enable);

    CalendarModel::HorizontalHeaderFormat horizontalHeaderFormat() const;
    void setHorizontalHeaderFormat(CalendarModel::HorizontalHeaderFormat format);

    CalendarModel::VerticalHeaderFormat verticalHeaderFormat() const;
    void setVerticalHeaderFormat(CalendarModel::VerticalHeaderFormat format);

    void setViewStyleSheet(const QString& styleSheet);
    void setFilter(IServiceFilter *pNewFilter);
    void setHolidays(IHolidays* holidayProvider);

    void redrawCellBackgrounds();

public slots:
    void selectDate(const QDate &date);
    void setDateRange(const QDate &min, const QDate &max);
    void setCurrentMonth(int year, int month);
    void jumpToNextMonth();
    void jumpToPreviousMonth();
    void jumpToNextYear();
    void jumpToPreviousYear();
    void jumpToSelectedDate();
    void jumpToToday();

signals:
    void selectionChanged(const QDate &date);
    void activated(const QDate &date);
    void currentMonthChanged(int year, int month);

private:
    class CalendarWidgetPrivate *d_ptr;
    Q_DECLARE_PRIVATE(CalendarWidget)
    Q_DISABLE_COPY(CalendarWidget)

    Q_PRIVATE_SLOT(d_func(), void slotChangeDate(const QDate &date, bool changeMonth))
    Q_PRIVATE_SLOT(d_func(), void editingFinished())
};

#endif

