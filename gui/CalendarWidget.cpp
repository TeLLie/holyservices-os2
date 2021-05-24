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

#include "CalendarWidget.h"

#include "CalendarWidgetPrivate.h"
#include "CalendarView.h"
#include "CalendarDelegate.h"

#include <QHeaderView>
#include <QDate>
#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>

#include <qdebug.h>


/*!
    \class CalendarWidget

    \brief The CalendarWidget class provides a monthly based
    calendar widget allowing the user to select a date.

    \image CalendarWidget.png

    The widget is initialized with the current month and year, but
    CalendarWidget provides several public slots to change the year
    and month that is shown.  The currently displayed month and year
    can be retrieved using the currentMonth() and currentYear() functions,
    respectively.

    By default, today's date is selected, and the user can
    select a date using both mouse and keyboard. The currently
    selected date can be retrieved using the selectedDate()
    function. It is possible to constrain the user selection to a
    given date range by setting the minimumDate and maximumDate
    properties. Alternatively, both properties can be set in one go
    using the setDateRange() convenience slot. Set the \l selectable
    property to false to prohibit the user from selecting at all. Note
    that a date also can be selected programmatically using the
    selectDate() slot.

    A newly created calendar widget has single letter notation for the
    days (M for Mondays), and both Sundays and today's date are
    marked. The calendar table is completed, i.e. days from the
    previous and next months are added to avoid empty cells, but the
    calendar grid is not shown. The week numbers are displayed, and
    the first column day is Sunday.

    All this properties can be altered using the corresponding set
    functions.

    The notation of the days can be altered to a longer abbreviation
    (Mon for Monday) by setting the horizontalHeaderFormat property to
    CalendarWidget::ShortDayNames. Setting the same property to
    CalendarWidget::LongDayNames makes the header display the
    complete day names. Set the showSundaysInRed and todayMarked
    properties to false to remove the mark-up colors. To prohibit
    completing days from the previous and next months, set the
    completingDaysShown property to false.  The week numbers can be
    removed by setting the verticalHeaderFormat property to
    CalendarWidget::NoVerticalHeader.  The calendar grid can be
    turned on by setting the showGrid property to true using the
    setShowGrid() function:

    \table
    \row \o
        \image CalendarWidget-grid.png
    \row \o
        \code
            CalendarWidget *calendar;

            calendar->setShowGrid(true);
        \endcode
    \endtable

    Finally, the day in the first column can be altered using the
    setFirstDayOfWeek() function.

    The CalendarWidget class also provides three signals,
    selectionChanged(), activated() and currentMonthChanged() making it
    possible to respond to user interaction.

    \sa QDate, QDateEdit
*/

/*!
    Constructs a calendar widget with the given \a parent.

    The widget is initialized with the current month and year, and the
    currently selected date is today.

    \sa setCurrentMonth()
*/
CalendarWidget::CalendarWidget(QWidget *parent)
    : QWidget(parent)
{
    d_ptr = new CalendarWidgetPrivate;
    d_ptr->q_ptr = this;

    QVBoxLayout *layoutV = new QVBoxLayout(this);
    layoutV->setMargin(0);
    d_ptr->m_model = new CalendarModel(this);
    QFont f = font();
    f.setPointSize(f.pointSize() - 3);
    d_ptr->m_model->setCompletingDaysFont(f);
    d_ptr->m_view = new CalendarView(this);
    
    d_ptr->m_view->setItemDelegate(new CalendarDelegate(this));
    
	d_ptr->m_view->setModel(d_ptr->m_model);
    d_ptr->m_view->setSelectionBehavior(QAbstractItemView::SelectItems);
    d_ptr->m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    d_ptr->m_view->horizontalHeader()->setSectionsClickable(false);
    d_ptr->m_view->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    d_ptr->m_view->verticalHeader()->setSectionsClickable(false);
    d_ptr->m_selection = d_ptr->m_view->selectionModel();
    d_ptr->update();
    setFocusProxy(d_ptr->m_view);

    connect(d_ptr->m_view, SIGNAL(changeDate(const QDate &, bool)),
            this, SLOT(slotChangeDate(const QDate &, bool)));
    connect(d_ptr->m_view, SIGNAL(editingFinished()),
            this, SLOT(editingFinished()));

    layoutV->addWidget(d_ptr->m_view);
}

/*!
   Destroys the calendar widget.
*/
CalendarWidget::~CalendarWidget()
{
    delete d_ptr;
}

/*!
    \property CalendarWidget::selectedDate
    \brief the currently selected date.

    The selected date must be within the date range specified by the
    minimumDate and maximumDate properties. By default, the selected
    date is the current date.

    \sa setDateRange()
*/

QDate CalendarWidget::selectedDate() const
{
    return d_ptr->date();
}

void CalendarWidget::selectDate(const QDate &date)
{
    if (d_ptr->date() == date)
        return;

    if (!date.isValid())
        return;

    d_ptr->setDate(date);
    QDate newDate = d_ptr->date();
    d_ptr->showMonth(newDate.year(), newDate.month());
}

/*!
    Returns the year of the currently displayed month.

    \sa currentMonth(), setCurrentMonth()
*/

int CalendarWidget::currentYear() const
{
    return d_ptr->m_model->shownYear();
}

/*!
    Returns the currently displayed month.

    \sa currentYear(), setCurrentMonth()
*/

int CalendarWidget::currentMonth() const
{
    return d_ptr->m_model->shownMonth();
}

/*!
    Displays the given \a month of the given \a year without changing
    the selected date. Use the selectDate() function to alter the
    selected date.

    The currently displayed month and year can be retrieved using the
    currentMonth() and currentYear() functions respectively.

    \sa currentYear(), jumpToPreviousMonth(), jumpToNextMonth(), jumpToPreviousYear(),
    jumpToNextYear()
*/

void CalendarWidget::setCurrentMonth(int year, int month)
{
    d_ptr->showMonth(year, month);
}

/*!
    Shows the next month relative to the currently displayed
    month. Note that the selected date is not changed.

    \sa jumpToPreviousMonth(), setCurrentMonth(), selectDate()
*/

void CalendarWidget::jumpToNextMonth()
{
    int year = currentYear();
    int month = currentMonth();
    if (month == 12) {
        year++;
        month = 1;
    } else {
        month += 1;
    }
    setCurrentMonth(year, month);
}

/*!
    Shows the previous month relative to the currently displayed
    month. Note that the selected date is not changed.

    \sa jumpToNextMonth(), setCurrentMonth(), selectDate()
*/

void CalendarWidget::jumpToPreviousMonth()
{
    int year = currentYear();
    int month = currentMonth();
    if (month == 1) {
        year--;
        month = 12;
    } else {
        month -= 1;
    }
    setCurrentMonth(year, month);
}

/*!
    Shows the currently displayed month in the \e next year relative
    to the currently displayed year. Note that the selected date is
    not changed.

    \sa jumpToPreviousYear(), setCurrentMonth(), selectDate()
*/

void CalendarWidget::jumpToNextYear()
{
    int year = currentYear();
    int month = currentMonth();
    year++;
    setCurrentMonth(year, month);
}

/*!
    Shows the currently displayed month in the \e previous year
    relative to the currently displayed year. Note that the selected
    date is not changed.

    \sa jumpToNextYear(), setCurrentMonth(), selectDate()
*/

void CalendarWidget::jumpToPreviousYear()
{
    int year = currentYear();
    int month = currentMonth();
    year--;
    setCurrentMonth(year, month);
}

/*!
    Shows the month of the selected date.

    \sa selectedDate(), setCurrentMonth()
*/

void CalendarWidget::jumpToSelectedDate()
{
    QDate currentDate = selectedDate();
    setCurrentMonth(currentDate.year(), currentDate.month());
}

/*!
    Shows the month of the today's date.

    \sa selectedDate(), setCurrentMonth()
*/

void CalendarWidget::jumpToToday()
{
    QDate currentDate = QDate::currentDate();
    setCurrentMonth(currentDate.year(), currentDate.month());
}

/*!
    \property CalendarWidget::minimumDate
    \brief the minimum date of the currently specified date range.

    The user will not be able to select a date which is before the
    currently set minimum date.

    \table
    \row
    \o \image CalendarWidget-minimum.png
    \row
    \o
    \code
    CalendarWidget *calendar;

    calendar->setShowGrid(true);
    calendar->setMinimumDate(QDate(2006, 1, 5));
    \endcode
    \endtable

    By default, the minimum date is the earliest date that the QDate
    class can handle, i.e. 14 September 1752.

    When setting a minimum date, the maximumDate and \l selectedDate
    properties are adjusted if the selection range becomes invalid. If
    the provided date is not a valid QDate object, the
    setMinimumDate() function does nothing.

    \sa setDateRange()
*/

QDate CalendarWidget::minimumDate() const
{
    return d_ptr->m_model->minimumDate();
}

void CalendarWidget::setMinimumDate(const QDate &date)
{
    if (d_ptr->m_model->minimumDate() == date)
        return;

    if (!date.isValid())
        return;

    QDate oldDate = d_ptr->date();
    d_ptr->m_model->setMinimumDate(date);
    QDate newDate = d_ptr->date();
    if (oldDate != newDate) {
        d_ptr->update();
        d_ptr->showMonth(newDate.year(), newDate.month());
        emit selectionChanged(newDate);
    }
}

/*!
    \property CalendarWidget::maximumDate
    \brief the maximum date of the currently specified date range.

    The user will not be able to select a date which is after the
    currently set maximum date.

    \table
    \row
    \o \image CalendarWidget-maximum.png
    \row
    \o
    \code
    CalendarWidget *calendar;

    calendar->setShowGrid(true);
    calendar->setMaximumDate(QDate(2006, 1, 20));
    \endcode
    \endtable

    By default, the maximum date is the last day the QDate class can
    handle, i.e. 31. December 7999.

    When setting a maximum date, the minimumDate and \l selectedDate
    properties are adjusted if the selection range becomes invalid. If
    the provided date is not a valid QDate object, the
    setMaximumDate() function does nothing.

    \sa setDateRange()
*/

QDate CalendarWidget::maximumDate() const
{
    return d_ptr->m_model->maximumDate();
}

void CalendarWidget::setMaximumDate(const QDate &date)
{
    if (d_ptr->m_model->maximumDate() == date)
        return;

    if (!date.isValid())
        return;

    QDate oldDate = d_ptr->date();
    d_ptr->m_model->setMaximumDate(date);
    QDate newDate = d_ptr->date();
    if (oldDate != newDate) {
        d_ptr->update();
        d_ptr->showMonth(newDate.year(), newDate.month());
        emit selectionChanged(newDate);
    }
}

/*!
    Defines a date range by setting the minimumDate and maximumDate
    properties.

    The date range restricts the user selection, i.e. the user can
    only select dates within the specified date range. Note that

    \code
        CalendarWidget *calendar;

        calendar->setDateRange(min, max);
    \endcode

    is analogous to

    \code
        CalendarWidget *calendar;

        calendar->setMinimumDate(min);
        calendar->setMaximumDate(max);
    \endcode

    If either the \a min or \a max parameters are not valid QDate
    objects, this function does nothing.

    \sa setMinimumDate(), setMaximumDate()
*/

void CalendarWidget::setDateRange(const QDate &min, const QDate &max)
{
    if (d_ptr->m_model->minimumDate() == min &&
            d_ptr->m_model->maximumDate() == max)
        return;

    if (!min.isValid())
        return;

    if (!max.isValid())
        return;

    QDate minimum = min;
    QDate maximum = max;
    if (min > max) {
        minimum = max;
        maximum = min;
    }

    QDate oldDate = d_ptr->date();
    d_ptr->m_model->setRange(min, max);
    QDate newDate = d_ptr->date();
    if (oldDate != newDate) {
        d_ptr->update();
        d_ptr->showMonth(newDate.year(), newDate.month());
        emit selectionChanged(newDate);
    }
}

/*!
    \property CalendarWidget::todayMarked
    \brief whether the current date (today) is marked.

    When this property is set to true, the current date is marked with
    a gray background color; otherwise there is no color background.

    The default value is true.
*/

void CalendarWidget::setTodayMarked(bool marked)
{
    if (isTodayMarked() == marked)
        return;

    d_ptr->m_model->setTodayMarked(marked);
    d_ptr->update();
}

bool CalendarWidget::isTodayMarked() const
{
    return d_ptr->m_model->isTodayMarked();
}

/*!
    \property CalendarWidget::showSundaysInRed
    \brief whether the Sundays are marked.

    When this property is set to true, Sundays are marked with a red
    color; otherwise they have the same formatting as the other days
    of the week.

    The default value is true.
*/

void CalendarWidget::setShowSundaysInRed(bool enable)
{
    if (showSundaysInRed() == enable)
        return;

    d_ptr->m_model->setSundaysMarked(enable);
    d_ptr->update();
}

bool CalendarWidget::showSundaysInRed() const
{
    return d_ptr->m_model->sundaysMarked();
}

/*! \enum CalendarWidget::HorizontalHeaderFormat

    This enum type defines the various formats the horizontal header can display.

    \value SingleLetterDayNames The header displays a single letter abbreviation for day names (e.g. M for Monday).
    \value ShortDayNames The header displays a short abbreviation for day names (e.g. Mon for Monday).
    \value LongDayNames The header displays complete day names (e.g. Monday).
    \value NoHorizontalHeader The header is hidden.

    \sa horizontalHeaderFormat(), VerticalHeaderFormat
*/

/*!
    \property CalendarWidget::horizontalHeaderFormat
    \brief the format of the horizontal header.

    The default value is CalendarWidget::SingleLetterDayNames.
*/

void CalendarWidget::setHorizontalHeaderFormat(CalendarModel::HorizontalHeaderFormat format)
{
    if (horizontalHeaderFormat() == format)
        return;

    d_ptr->m_model->setDayNamesFormat(format);
    if (format == CalendarModel::NoHorizontalHeader)
        d_ptr->m_view->setDayNamesShown(false);
    else
        d_ptr->m_view->setDayNamesShown(true);
    d_ptr->update();
}

CalendarModel::HorizontalHeaderFormat CalendarWidget::horizontalHeaderFormat() const
{
    return d_ptr->m_model->dayNamesFormat();
}

/*!
    \property CalendarWidget::completingDaysShown

    \brief whether the calendar table should be filled completely.

    When this property is set to true, days from the previous and next
    month are displayed to fill the calendar table; otherwise there
    may be empty cells in the top-left and bottom-right corner of the
    table.

    \table
    \row
        \o \image CalendarWidget-complete.png
        \o \image CalendarWidget-notcomplete.png
    \row
        \o
        \code
            CalendarWidget *calendar;

            calendar->setCompletingDaysShown(true);
        \endcode
        \o
        \code
            CalendarWidget *calendar;

            calendar->setCompletingDaysShown(false);
        \endcode
    \endtable

    The default value is true.
*/

void CalendarWidget::setCompletingDaysShown(bool show)
{
    if (completingDaysShown() == show)
        return;

    d_ptr->m_model->setCompletingDaysShown(show);
    d_ptr->update();
}

bool CalendarWidget::completingDaysShown() const
{
    return d_ptr->m_model->completingDaysShown();
}

/*! \enum CalendarWidget::VerticalHeaderFormat

    This enum type defines the various formats the vertical header can display.

    \value ISOWeekNumbers The header displays a ISO week numbers \l QDate::weekNumber().
    \value NoVerticalHeader The header is hidden.

    \sa verticalHeaderFormat(), HorizontalHeaderFormat
*/

/*!
    \property CalendarWidget::verticalHeaderFormat
    \brief the format of the vertical header.

    The default value is CalendarWidget::ISOWeekNumber.
*/

CalendarModel::VerticalHeaderFormat CalendarWidget::verticalHeaderFormat() const
{
    bool shown = d_ptr->m_view->weekNumbersShown();
    if (shown)
        return CalendarModel::ISOWeekNumbers;
    return CalendarModel::NoVerticalHeader;
}

void CalendarWidget::setVerticalHeaderFormat(CalendarModel::VerticalHeaderFormat format)
{
    bool shown = false;
    if (format == CalendarModel::ISOWeekNumbers)
        shown = true;
    d_ptr->m_view->setWeekNumbersShown(shown);
}

/*!
    \property CalendarWidget::showGrid
    \brief whether the table grid is displayed.

    \table
    \row
        \o \inlineimage CalendarWidget-grid.png
    \row
        \o
        \code
            CalendarWidget *calendar;

            calendar->setShowGrid(true);
        \endcode
    \endtable

    The default value is false.
*/

bool CalendarWidget::showGrid() const
{
    return d_ptr->m_view->showGrid();
}

void CalendarWidget::setShowGrid(bool show)
{
    d_ptr->m_view->setShowGrid(show);
    d_ptr->m_view->viewport()->update();
}

/*!
    \property CalendarWidget::selectable
    \brief whether the user can select a date in the calendar.

    When this property is set to true the user can select a date
    within the specified date range, using both mouse and
    keyboard. Otherwise the user is prohibited from selecting any
    date.

    The default value is true.
*/

bool CalendarWidget::isSelectable() const
{
    return !d_ptr->m_view->isReadOnly();
}

void CalendarWidget::setSelectable(bool enable)
{
    d_ptr->m_view->setReadOnly(!enable);
}

/*!
    \property CalendarWidget::firstDayOfWeek
    \brief a value identifying the day displayed in the first column.

    By default, the day displayed in the first column is Sunday
*/

void CalendarWidget::setFirstDayOfWeek(Qt::DayOfWeek dayOfWeek)
{
    if (firstDayOfWeek() == dayOfWeek)
        return;

    d_ptr->m_model->setFirstColumnDay((int)dayOfWeek);
    d_ptr->update();
}

Qt::DayOfWeek CalendarWidget::firstDayOfWeek() const
{
    return (Qt::DayOfWeek)d_ptr->m_model->firstColumnDay();
}

/*!
    \fn void CalendarWidget::selectionChanged(const QDate &date)

    This signal is emitted when the currently selected date is
    changed, passing the new \a date as parameter.

    The currently selected date can be changed by the user using the
    mouse or keyboard, or by the programmer using the selectDate()
    function.

    \sa selectDate()
*/

/*!
    \fn void CalendarWidget::activated(const QDate &date)

    This signal is emitted whenever the user presses the Return or
    Enter key, the space bar or double-clicks a \a date in the calendar
    widget.
*/

/*!
    \fn void CalendarWidget::currentMonthChanged(int year, int month)

    This signal is emitted when the currently shown month is changed,
    passing the new \a month and \a year as parameters.

    \sa setCurrentMonth()
*/

void CalendarWidget::setViewStyleSheet(const QString& styleSheet)
{
	d_ptr->m_view->setStyleSheet(styleSheet);
}

void CalendarWidget::setFilter(IServiceFilter *pNewFilter)
{
	d_ptr->m_model->setFilter(pNewFilter);
}

void CalendarWidget::setHolidays(IHolidays* holidayProvider)
{
    d_ptr->m_model->setHolidays(holidayProvider);
}

void CalendarWidget::redrawCellBackgrounds()
{
    d_ptr->m_model->redrawCellBackgrounds();
}
