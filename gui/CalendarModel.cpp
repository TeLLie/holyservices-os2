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

#include "CalendarModel.h"

#include "HolyServiceIndexing.h"
#include "HolyService.h"
#include "AssignedFilter.h"
#include "MovableServiceFilter.h"
#include "ChurchIndexing.h"
#include "IHolidays.h"

#include <QColor>
#include <QBrush>
#include <QIcon>
#include <QtDebug>
#include <QLocale>

CalendarModel::CalendarModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_date = QDate::currentDate();
    m_minimumDate = QDate(1752, 9, 14);
    m_maximumDate = QDate(7999, 12, 31);
    m_shownYear = m_date.year();
    m_shownMonth = m_date.month();
    m_completingDaysFont.setPointSize(m_completingDaysFont.pointSize() - 3);
}

void CalendarModel::setCompletingDaysFont(const QFont &font)
{
    m_completingDaysFont = font;
    internalUpdate();
}

int CalendarModel::dayOfWeekForColumn(int column) const
{
    if (column < 0 || column > 6) return 0;
    auto day = m_fixedStartingDay + column;
    if (day > 7) day -= 7;
    return day;
}

int CalendarModel::columnForDayOfWeek(int day) const
{
    if (day < 1 || day > 7) return -1;
    auto column = day - m_fixedStartingDay;
    if (column < 0) column += 7;
    return column;
}

QDate CalendarModel::dateForCell(int row, int column) const
{
    if (row < 0 || row > 5 || column < 0 || column > 6)
        return QDate();
    QDate firstDate(m_shownYear, m_shownMonth, 15);
    if (!firstDate.isValid()) {
        return QDate();
    }
    int columnForFirstOfShownMonth = columnForDayOfWeek(firstDate.dayOfWeek());
    if (row == 0 && column < columnForFirstOfShownMonth) {
        QDate previousMonth = firstDate.addMonths(-1);
        if (!previousMonth.isValid()) {
            return QDate();
        }
        int daysInMonth = previousMonth.daysInMonth();
        return QDate(previousMonth.year(), previousMonth.month(),
                    daysInMonth - columnForFirstOfShownMonth + column + 1);
    }
    int daysInShownMonth = firstDate.daysInMonth();
    int requestedDay = 7 * row + column - columnForFirstOfShownMonth + 1;
    if (requestedDay > daysInShownMonth) {
        QDate nextMonth = firstDate.addMonths(1);
        if (!nextMonth.isValid()) {
            return QDate();
        }
        return QDate(nextMonth.year(), nextMonth.month(),
                    requestedDay - daysInShownMonth);
    }
    return QDate(m_shownYear, m_shownMonth, requestedDay);
}

void CalendarModel::cellForDate(const QDate &date, int &row, int &column) const
{
    row = -1;
    column = -1;

    int day = date.day();
    QDate firstDate(m_shownYear, m_shownMonth, 15);
    int columnForFirstOfShownMonth = columnForDayOfWeek(firstDate.dayOfWeek());
    int daysInShownMonth = firstDate.daysInMonth();

    QDate previousMonth = firstDate.addMonths(-1);
    QDate nextMonth = firstDate.addMonths(1);

    if (date.year() == m_shownYear && date.month() == m_shownMonth) {
        column = (day + columnForFirstOfShownMonth - 1) % 7;
        row = (day + columnForFirstOfShownMonth - 1) / 7;
    } else if (previousMonth.isValid() &&
            date.year() == previousMonth.year() && date.month() == previousMonth.month()) {
        int daysInMonth = previousMonth.daysInMonth();
        int col = day - daysInMonth + columnForFirstOfShownMonth - 1;
        if (col >= 0) {
            column = col;
            row = 0;
        }
    } else if (nextMonth.isValid() &&
            date.year() == nextMonth.year() && date.month() == nextMonth.month()) {
        int r = (day + daysInShownMonth + columnForFirstOfShownMonth - 1) / 7;
        if (r <= 5) {
            column = (day + daysInShownMonth + columnForFirstOfShownMonth - 1) % 7;
            row = r;
        }
    }
}

QDate CalendarModel::visibleDateForCell(int row, int column) const
{
    QDate date = dateForCell(row, column);
    if (m_completingDaysShown || (date.month() == m_shownMonth && date.year() == m_shownYear))
        return date;
    return QDate();
}

void CalendarModel::cellForVisibleDate(const QDate &date, int &row, int &column) const
{
    if (m_completingDaysShown || (date.month() == m_shownMonth && date.year() == m_shownYear)) {
        cellForDate(date, row, column);
    } else {
        row = -1;
        column = -1;
    }
}

QVariant CalendarModel::data(const QModelIndex &index, int role) const
{
    const QDate date = visibleDateForCell(index.row(), index.column());

	if (date.isValid()) {
		switch (role)
		{
		case Qt::DisplayRole: return date.day();
		case Qt::TextAlignmentRole:
			{
				int align = Qt::AlignRight | Qt::AlignVCenter;
				return align;
			}
		case Qt::FontRole:
            if (m_shownMonth != date.month()) {
                return m_completingDaysFont;
            }
			break;
		case Qt::ForegroundRole:
            if (m_sundaysMarked && date.dayOfWeek() == 7 &&
                        date >= m_minimumDate && date <= m_maximumDate) {
                return QColor(Qt::red);
            }
			break;
		case Qt::BackgroundRole:
			{
				// today is irelevant
				//if (m_todayMarked && date == today) {
				//    return QColor(Qt::gray);
				//}

	            if (m_shownMonth == date.month()) 
				{
                    const auto allAtDate = HolyServiceIndexing::allSet().query(date, date, m_filter);
                    return brushForServices(allAtDate);
                }
			}
			break;
		case Qt::DecorationRole:
            {
                tHolyServiceVector allAtDate;

                allAtDate = HolyServiceIndexing::allSet().query(date, date, m_filter);
                if (! allAtDate.isEmpty())
                {
                    static const MovableServiceFilter movableFilter;
                    tHolyServiceVector movableAtDate = HolyServiceIndexing::filter(allAtDate, &movableFilter);
                    if (! movableAtDate.isEmpty())
                    {
                        static QPixmap movableIndicator = QPixmap(":/images/ledlightblue.png").
                                scaled(8, 8, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                        return movableIndicator;
                    }
                }
            }
			break;
		case Qt::ToolTipRole:
            return dayTooltip(date);
		}
    }
    return QVariant();
}

QVariant CalendarModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (section >= 0 && section <= 6) {
            if (role == Qt::DisplayRole) {
				QLocale myLocale;

                switch (m_dayNamesFormat) {
                    case SingleLetterDayNames:
						return myLocale.dayName(dayOfWeekForColumn(section), QLocale::ShortFormat).left(1);
                    case ShortDayNames:
                        return myLocale.dayName(dayOfWeekForColumn(section), QLocale::ShortFormat);
                    case LongDayNames:
                        return myLocale.dayName(dayOfWeekForColumn(section), QLocale::LongFormat);
                    default:
                        return QString();
                }
            } else if (role == Qt::TextColorRole) {
                if (m_sundaysMarked && dayOfWeekForColumn(section) == 7) {
                    return QColor(Qt::red);
                }
            } else if (role == Qt::TextAlignmentRole) {
                int align = Qt::AlignHCenter | Qt::AlignTop;
                return align;
            }
        }
    } 
	else 
	{
        if (section >= 0 && section <= 5) {
            if (role == Qt::DisplayRole) {
                QDate date = dateForCell(section, columnForDayOfWeek(1));
                if (date.isValid()) {
                    return date.weekNumber();
                }
            }
            if (role == Qt::TextAlignmentRole) {
                int align = Qt::AlignRight | Qt::AlignVCenter;
                return align;
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags CalendarModel::flags(const QModelIndex &index) const
{
    QDate date = dateForCell(index.row(), index.column());
    if (!date.isValid()) return Qt::ItemFlags{};
    if (date < m_minimumDate) return Qt::ItemFlags{};
    if (date > m_maximumDate) return Qt::ItemFlags{};
    return QAbstractTableModel::flags(index);
}

void CalendarModel::setDate(const QDate &date)
{
    if (!date.isValid())
        return;
    if (m_date == date)
        return;

    m_date = round(date);
}

void CalendarModel::showMonth(int year, int month)
{
    if (m_shownYear == year && m_shownMonth == month)
        return;

    m_shownYear = year;
    m_shownMonth = month;

    internalUpdate();
}

QDate CalendarModel::date() const
{
    return m_date;
}

QDate CalendarModel::round(const QDate &date) const
{
    QDate d = date;
    if (d < m_minimumDate)
        d = m_minimumDate;
    else if (d > m_maximumDate)
        d = m_maximumDate;
    return d;
}

QDate CalendarModel::minimumDate() const
{
    return m_minimumDate;
}

void CalendarModel::setMinimumDate(const QDate &date)
{
    if (!date.isValid())
        return;
    if (date == m_minimumDate)
        return;

    m_minimumDate = date;
    if (m_maximumDate < m_minimumDate)
        m_maximumDate = m_minimumDate;
    if (m_date < m_minimumDate)
        m_date = m_minimumDate;
    internalUpdate();
}

QDate CalendarModel::maximumDate() const
{
    return m_maximumDate;
}

void CalendarModel::setMaximumDate(const QDate &date)
{
    if (!date.isValid())
        return;
    if (date == m_maximumDate)
        return;

    m_maximumDate = date;
    if (m_minimumDate > m_maximumDate)
        m_minimumDate = m_maximumDate;
    if (m_date > m_maximumDate)
        m_date = m_maximumDate;
    internalUpdate();
}

void CalendarModel::setRange(const QDate &min, const QDate &max)
{
    m_minimumDate = min;
    m_maximumDate = max;
    if (m_date < m_minimumDate)
        m_date = m_minimumDate;
    if (m_date > m_maximumDate)
        m_date = m_maximumDate;
    internalUpdate();
}

int CalendarModel::shownYear() const
{
    return m_shownYear;
}

int CalendarModel::shownMonth() const
{
    return m_shownMonth;
}

void CalendarModel::internalUpdate()
{
    QModelIndex begin = index(0, 0);
    QModelIndex end = index(5, 6);
    emit dataChanged(begin, end);
    emit headerDataChanged(Qt::Vertical, 0, 5);
    emit headerDataChanged(Qt::Horizontal, 0, 6);
}

void CalendarModel::setTodayMarked(bool marked)
{
    if (m_todayMarked == marked)
        return;

    m_todayMarked = marked;
    internalUpdate();
}

bool CalendarModel::isTodayMarked() const
{
    return m_todayMarked;
}

void CalendarModel::setSundaysMarked(bool marked)
{
    if (m_sundaysMarked == marked)
        return;

    m_sundaysMarked = marked;
    internalUpdate();
}

bool CalendarModel::sundaysMarked() const
{
    return m_sundaysMarked;
}

void CalendarModel::setDayNamesFormat(HorizontalHeaderFormat format)
{
    if (m_dayNamesFormat == format)
        return;

    m_dayNamesFormat = format;
    internalUpdate();
}

CalendarModel::HorizontalHeaderFormat CalendarModel::dayNamesFormat() const
{
    return m_dayNamesFormat;
}

void CalendarModel::setCompletingDaysShown(bool shown)
{
    if (m_completingDaysShown == shown)
        return;

    m_completingDaysShown = shown;
    internalUpdate();
}

bool CalendarModel::completingDaysShown() const
{
    return m_completingDaysShown;
}

void CalendarModel::setFirstColumnDay(int dayOfWeek)
{
    if (dayOfWeek < 1)
        dayOfWeek = 1;
    else if (dayOfWeek > 7)
        dayOfWeek = 7;
    if (m_fixedStartingDay == dayOfWeek)
        return;

    m_fixedStartingDay = dayOfWeek;
    internalUpdate();
}

int CalendarModel::firstColumnDay() const
{
    return m_fixedStartingDay;
}

void CalendarModel::setFilter(IServiceFilter* p)
{
    m_filter = p;
    internalUpdate();
}

void CalendarModel::setHolidays(IHolidays* p)
{
    m_holidays = p;
    internalUpdate();
}

void CalendarModel::redrawCellBackgrounds()
{
    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() -1), {Qt::BackgroundRole});
}

struct ServiceAssignment
{
    bool hasAssignedService = false;
    bool hasUnassignedService = false;
};

QBrush CalendarModel::brushForServices(const tHolyServiceVector& allAtDate)
{
    if (allAtDate.isEmpty()) return QBrush();

    // assemble list of churches for services at date
    QHash<Church*, ServiceAssignment> churchAssignments;

    foreach (Church* pChurch, ChurchIndexing::allSet().data())
	{
		foreach(HolyService* pService, allAtDate)
		{
			if (pService->getChurch() == pChurch)
			{
                if (AssignedFilter::isServiceAssigned(*pService))
                {
                    churchAssignments[pChurch].hasAssignedService = true;
                }
                else
                {
                    churchAssignments[pChurch].hasUnassignedService = true;
                }
			}
		}
	}

    if (churchAssignments.isEmpty())
	{
		return QBrush();
	}

    auto nameSortedChurches = churchAssignments.keys();
    std::sort(nameSortedChurches.begin(), nameSortedChurches.end(),
              [](Church* c1, Church* c2) { return c1->name() < c2->name(); });

    QVector<QColor> brushColors;
    for (const auto & church : nameSortedChurches)
    {
        const auto assignment = churchAssignments.value(church);
        if (assignment.hasAssignedService)
        {
            brushColors.append(church->color());
        }
        if (assignment.hasUnassignedService)
        {
            brushColors.append(church->color().lighter(150));
        }
    }

	if (brushColors.size() == 1)
	{
		return QBrush(brushColors.first());
	}

	// fill the gradient
	QLinearGradient gradient(0.0, 0.0, 0.0, 24.0);

	gradient.setSpread(QGradient::RepeatSpread);

	const double gradientStep = 1.0 / brushColors.count();
	for (int index = 0; index < brushColors.count(); ++index)
	{
		const QColor& churchColor = brushColors.at(index);

		gradient.setColorAt(index * gradientStep, churchColor);
		gradient.setColorAt(((index + 1) * gradientStep) - 0.0001, churchColor);
		//qDebug() << "Adding color " << brushColors.at(index) << " on position " << index * gradientStep;
	}
//	gradient.setColorAt(1.0, brushColors.last());
	return QBrush(gradient);
}

QString CalendarModel::dayTooltip(const QDate& date) const
{
    QString result;
    QStringList holidayDesc;
    if (m_holidays && m_holidays->isHoliday(date, &holidayDesc) &&
        (!holidayDesc.isEmpty()))
    {
        result.append(QString("<b>%1</b>").arg(holidayDesc.join("<br>")));
    }
    const tHolyServiceVector allAtDate =
            HolyServiceIndexing::allSet().query(date, date, m_filter);

    if (allAtDate.size())
    {
        result.append("<ul>");
    }
    foreach(const HolyService* const hs, allAtDate)
    {
        result += "<li>";
        result += hs->toString(HolyService::ShortFormat);
        result += "</li>";
    }
    if (allAtDate.size())
    {
        result.append("</ul>");
    }
    return result;
}
