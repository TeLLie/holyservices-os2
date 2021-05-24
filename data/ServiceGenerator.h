#ifndef SERVICEGENERATOR_H_INCLUDED_B8C27B18_E275_43EB_AF24_07B80A74A9D5
#define SERVICEGENERATOR_H_INCLUDED_B8C27B18_E275_43EB_AF24_07B80A74A9D5

///\class ServiceGenerator
/// backend for GUI of service generator - generates holy service every
/// n-th week or every n-th eekday in date span
///\date 10-2006
///\author Jan 'Kovis' Struhar

#include "HsTypedefs.h"
#include <QDate>
#include <QTime>
#include <Qt>

class Church;
class Priest;
class HolyService;
class Client;

class ServiceGenerator
{
public:
	static bool test();

        ServiceGenerator();
        virtual ~ServiceGenerator();

	/// setup methods
	void setStableData(const Church* church, const Priest* celebrant, 
			const Client* client,
			const QTime& timeFrom, const QTime& timeTo, 
			const QDate& dateFrom, const QDate& dateTo, 
			const QString& intention);

	tHolyServiceVector generateEveryWeekInMonth(const int weekOrder, 
		const Qt::DayOfWeek weekDay, const QDate& startCountingFrom);

	tHolyServiceVector generateOneWeekdayInMonth(const int weekDayOrder, 
		const Qt::DayOfWeek weekDay);

	tHolyServiceVector generateForStableChurch();

private:
	//@{
	///\name stable data
	const Church *m_church;
	const Priest *m_priest;
	const Client *m_client;
	QDate m_dateFrom;
	QDate m_dateTo;
	QTime m_timeFrom;
	QTime m_timeTo;
	QString m_intention;
	//@}

	HolyService* createService(const QDate& date);
	bool dateAndTimeRangeOk() const;
};

#endif
