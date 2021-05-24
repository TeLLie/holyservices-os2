#pragma once

#include <QWidget>
#include <memory>

class QDate;
class QLabel;
class QProgressBar;
class CalendarWidget;
class IServiceFilter;
class IHolidays;
class HolyService;

///\class ServiceMonthWidget
/// Display overview about one calendar month - calendar, label with month name 
/// and percentage of unassigned services in the month
///\author Jan 'Kovis' Struhar

class ServiceMonthWidget : public QWidget
{
	typedef QWidget super;

	Q_OBJECT
public:
    ServiceMonthWidget(QWidget *parent);

	void setMonth(int month, int year);
    void setFilter(IServiceFilter *pNewFilter);
    void setHolidays(IHolidays* holidayProvider);

public slots:
	/// updates indicator (progress bar) of occupied services in month
	void updateOccupied();

private slots:
	void editDate(const QDate& date);
	void showContextMenu(const QPoint&);
    void handleDelete();

private:
	QLabel *m_monthLabel;
	QProgressBar *m_occupied;
    CalendarWidget *m_monthCalendar;
	IServiceFilter *m_serviceFilter; ///< filters all queries
    IHolidays* m_holidayProvider;

    std::unique_ptr<HolyService> createService(const QDate&);

	/// work around incorrect month data in Qt 4.3.2.
	QString monthName(int month) const;

    void addHoliday(const QDate& date, const QString& text, bool forNextYears);

    void saveServiceChanges();
};
