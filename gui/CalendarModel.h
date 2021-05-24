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

#ifndef CALENDAR_MODEL_H_INCLUDED_0E1D0548_5EF9_4738_9C21_0228DB38D84F
#define CALENDAR_MODEL_H_INCLUDED_0E1D0548_5EF9_4738_9C21_0228DB38D84F

#include <QAbstractTableModel>
#include <QDate>
#include <QFont>

#include "HsTypedefs.h"

class IServiceFilter;
class IHolidays;

class CalendarModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum HorizontalHeaderFormat 
	{
        SingleLetterDayNames,
        ShortDayNames,
        LongDayNames,
        NoHorizontalHeader
    };

    enum VerticalHeaderFormat 
	{
        ISOWeekNumbers,
//        TraditionalWeekNumbers,
        NoVerticalHeader
    };

    CalendarModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex& = QModelIndex()) const override
        { return 6; /* max number of weeks in month */}
    int columnCount(const QModelIndex& = QModelIndex()) const override
        { return 7; /* days in week */ }
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setDate(const QDate &date);
    void showMonth(int year, int month);

    QDate date() const;
    int shownYear() const;
    int shownMonth() const;

    QDate minimumDate() const;
    void setMinimumDate(const QDate &date);

    QDate maximumDate() const;
    void setMaximumDate(const QDate &date);

    void setRange(const QDate &min, const QDate &max);

    QDate round(const QDate &date) const;

    void setTodayMarked(bool marked);
    bool isTodayMarked() const;

    void setSundaysMarked(bool marked);
    bool sundaysMarked() const;

    void setDayNamesFormat(HorizontalHeaderFormat format);
    HorizontalHeaderFormat dayNamesFormat() const;

    void setCompletingDaysShown(bool show);
    bool completingDaysShown() const;

    void setFirstColumnDay(int dayOfWeek);
    int firstColumnDay() const;

    int dayOfWeekForColumn(int section) const;
    int columnForDayOfWeek(int day) const;
    QDate dateForCell(int row, int column) const;
    void cellForDate(const QDate &date, int &row, int &column) const;
    QDate visibleDateForCell(int row, int column) const;
    void cellForVisibleDate(const QDate &date, int &row, int &column) const;

    void setCompletingDaysFont(const QFont &font);

	void setFilter(IServiceFilter*);
    void setHolidays(IHolidays*);

    void redrawCellBackgrounds();

private:
    void internalUpdate();

	QDate m_date;
    QDate m_minimumDate;
    QDate m_maximumDate;
    int m_shownYear;
    int m_shownMonth;
    int m_fixedStartingDay = 7;
    bool m_todayMarked = true;
    bool m_sundaysMarked = true;
    HorizontalHeaderFormat m_dayNamesFormat = SingleLetterDayNames;
    bool m_completingDaysShown = true;
    QFont m_completingDaysFont;

    IServiceFilter* m_filter = nullptr;
    IHolidays* m_holidays = nullptr;

    static QBrush brushForServices(const tHolyServiceVector& allAtDate);
    QString dayTooltip(const QDate& date) const;
};

#endif

