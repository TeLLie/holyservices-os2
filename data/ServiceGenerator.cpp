/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ServiceGenerator.h"

#include "HolyService.h"
#include "Priest.h"
#include "Client.h"
#include "Church.h"

#include <QtDebug>
#include <assert.h>

ServiceGenerator::ServiceGenerator()
{
	m_church = nullptr;
	m_priest = nullptr;
	m_client = nullptr;
}

ServiceGenerator::~ServiceGenerator()
{
}

void ServiceGenerator::setStableData(const Church* church, const Priest* celebrant, 
		const Client* client,
		const QTime& timeFrom, const QTime& timeTo, 
		const QDate& dateFrom, const QDate& dateTo, 
		const QString& intention)
{
	assert(dateFrom < dateTo);
	assert(timeFrom < timeTo);

	m_church = church;
	m_priest = celebrant;
	m_client = client;
	m_dateFrom = dateFrom;
	m_dateTo = dateTo;
	m_timeFrom = timeFrom;
	m_timeTo = timeTo;
	m_intention = intention;
}

tHolyServiceVector ServiceGenerator::generateEveryWeekInMonth(const int weekOrder, 
	const Qt::DayOfWeek weekDay, const QDate& startCountingFrom)
{
	assert(weekOrder > 0);
	assert(startCountingFrom < m_dateTo);

	if (! dateAndTimeRangeOk())
	{
		return tHolyServiceVector();
	}

	tHolyServiceVector retVal;
	const int daysInOneWeek = 7;

	QDate startDay = startCountingFrom;

	// find a weekday
	while (weekDay != startDay.dayOfWeek())
	{
		startDay = startDay.addDays(1);
	}

	// and add weeks
	QDate currentDay;

	for (currentDay = startDay; currentDay <= m_dateTo; 
		currentDay = currentDay.addDays(weekOrder * daysInOneWeek))
	{
		if (currentDay == qBound(m_dateFrom, currentDay, m_dateTo))
		{
			retVal.append(createService(currentDay));
//			qDebug() << "WeekDay: " << currentDay.dayOfWeek() << ", " << currentDay;
		}
	}
	return retVal;
}

tHolyServiceVector ServiceGenerator::generateOneWeekdayInMonth(const int weekDayOrder, 
	const Qt::DayOfWeek weekDay)
{
	if (! dateAndTimeRangeOk())
	{
		return tHolyServiceVector();
	}

	tHolyServiceVector retVal;
	QDate currentDay(m_dateFrom.year(), m_dateFrom.month(), 1);
	int monthOfPreviousDay = currentDay.month();
	int weekDayIndex = 0;

	for (; currentDay < m_dateTo; currentDay = currentDay.addDays(1))
	{
		if (monthOfPreviousDay != currentDay.month())
		{
			weekDayIndex = 0;
		}

		if (currentDay.dayOfWeek() == weekDay)
		{
			++weekDayIndex;
		}

		if (weekDayOrder == weekDayIndex)
		{
			if (currentDay == qBound(m_dateFrom, currentDay, m_dateTo))
			{
				retVal.append(createService(currentDay));
//				qDebug() << "WeekDay: " << currentDay.dayOfWeek() << ", " << currentDay;
			}
			// no other match in this month
			weekDayIndex += 100000;
		}

		monthOfPreviousDay = currentDay.month();
	}
	return retVal;
}

HolyService* ServiceGenerator::createService(const QDate& date)
{
	HolyService* pService = new HolyService;

	pService->setId(IdTag::createInitialized());
	pService->setStartTime(QDateTime(date, m_timeFrom));
	pService->setEndTime(QDateTime(date, m_timeTo));
	pService->setIntention(m_intention);

	if (m_church)
	{
		pService->setChurchId(m_church->getId());
	}
	if (m_client)
	{
		pService->setClientId(m_client->getId());
	}
	if (m_priest)
	{
		pService->setPriestId(m_priest->getId());
	}
	return pService;
}

bool ServiceGenerator::test()
{
	return false;
}

bool ServiceGenerator::dateAndTimeRangeOk() const
{
	return (m_dateFrom <= m_dateTo) && (m_timeFrom < m_timeTo);
}

tHolyServiceVector ServiceGenerator::generateForStableChurch()
{
	if ((m_church == nullptr) || (! dateAndTimeRangeOk()))
	{
		return tHolyServiceVector();
	}

	tHolyServiceVector retVal;
	const Church::tServiceTemplateList& usualServices = m_church->usualServices();

	for (QDate currentDay = m_dateFrom; currentDay <= m_dateTo; 
		currentDay = currentDay.addDays(1))
	{
		foreach(const Church::ServiceTemplate& oneTemplate, usualServices)
		{
                        if (oneTemplate.m_active && (oneTemplate.weekDay() == currentDay.dayOfWeek()))
			{
				HolyService* pService = new HolyService;

				pService->setId(IdTag::createInitialized());
				pService->setStartTime(QDateTime(currentDay, oneTemplate.m_start));
				pService->setEndTime(pService->startTime().
					addSecs(oneTemplate.m_durationInMinutes * 60));
				if (m_church)
				{
					pService->setChurchId(m_church->getId());
				}
				pService->setPriestId(oneTemplate.m_priest);

				// and add finally
				retVal.append(pService);
			}
		}
	}
	return retVal;
}
