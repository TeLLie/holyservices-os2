/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "HolyService.h"

#include "ClientIndexing.h"
#include "ChurchIndexing.h"
#include "PriestIndexing.h"
#include "Client.h"
#include "Priest.h"
#include "Church.h"

#include <QObject>

HolyService::HolyService(void)
{
	setClean();
    m_timeConstraint = FixedToDate;
    m_receivedMoney = 0.0;
}

HolyService::~HolyService(void)
{
}

void HolyService::setIntention(const QString& anIntention)
{
	if (m_intention != anIntention)
	{
		setDirty();
		m_intention = anIntention;
	}
}

const QString& HolyService::intention() const
{
    return m_intention;
}

bool HolyService::hasIntention() const
{
    return (! m_intention.trimmed().isEmpty());
}

void HolyService::setStartTime(const QDateTime& aStartingTime)
{
	if (m_startTime != aStartingTime)
	{
		m_startTime = aStartingTime;
		setDirty();
	}
}

const QDateTime& HolyService::startTime() const
{
	return m_startTime;
}

void HolyService::setEndTime(const QDateTime& anEndTime)
{
	if (m_endTime != anEndTime)
	{
		m_endTime = anEndTime;
		setDirty();
	}
}

const QDateTime& HolyService::endTime() const
{
    return m_endTime;
}

void HolyService::moveInTime(const std::chrono::minutes& offset)
{
    if (offset.count() == 0) return;

    const std::chrono::seconds offsetInSeconds = offset;
    m_startTime = m_startTime.addSecs(offsetInSeconds.count());
    m_endTime = m_endTime.addSecs(offsetInSeconds.count());
    setDirty();
}

bool HolyService::conflicts(const HolyService& another, QString* report) const
{
	if (this == &another) // self conflict
	{
		return true;
	}

	const bool samePriest = (! m_priestId.isValid()) || 
		(! another.m_priestId.isValid()) || 
		(m_priestId == another.m_priestId);
	const bool sameChurch = another.m_churchId == m_churchId;
	const bool thisIsSooner = (endTime() <= another.startTime());
	const bool thisIsLater = (startTime() > another.endTime());
	const bool timeIntersects = (! (thisIsSooner || thisIsLater));
	const bool retVal = samePriest && sameChurch && timeIntersects;

	if (report && retVal)
	{
		*report = QObject::tr("Service\n\t%1\n collides with service\n\t%2").
			arg(this->toString(), another.toString());
	}
	return retVal;
}

QString HolyService::toString(TextFormat format) const
{
	QString retVal;

        switch (format)
        {
        case ShortFormat:
            return QObject::tr("%1-%2 : %3", "1 - start time, 2 - end time, 3- intention").
                    arg(startTime().time().toString("h:mm"),
                        endTime().time().toString("h:mm"),
                        intention());
        case DefaultFormat:
        default:
            return QObject::tr("Holy service %1 %2-%3 with intention %4",
                    "1 - date, 2 - start time, 3 - end time, 4 - intention").
                    arg(startTime().date().toString(Qt::LocalDate),
                    startTime().time().toString("h:mm"),
                    endTime().time().toString("h:mm"),
                    intention());
        }
}

bool HolyService::isValid() const
{
	const bool priestAssigned = true;
	const bool churchAssigned = true;
	const int secsInMinute = 60;
	const int serviceLengthMinutes = lengthSeconds() / secsInMinute;
	const bool timeIsOk = (serviceLengthMinutes >= 5) && (serviceLengthMinutes <= 600);

	return (priestAssigned && churchAssigned && timeIsOk);
}

bool HolyService::hasClient() const
{
	return m_clientId.isValid();
}

Client * HolyService::getClient() const
{
    Client *retVal = nullptr;
	if (hasClient())
	{
        retVal = ClientIndexing::allSet().find(m_clientId);
	}
	return retVal;
}

QString HolyService::clientText() const
{
    const Client *p = getClient();
	if (p)
	{
        return p->toString(Client::SurnameStreet);
	}
	return QObject::tr("<none>", "no client");
}

void HolyService::setClientId(const IdTag& clientId)
{
	if (clientId != m_clientId)
	{
		setDirty();
		m_clientId = clientId;
	}
}

void HolyService::setClient(const Client* const pClient)
{
	IdTag id;

	if (pClient) 
	{
		id = pClient->getId();
	}
	setClientId(id);
}

bool HolyService::hasChurch() const
{
	return m_churchId.isValid();
}

Church* HolyService::getChurch() const
{
    return ChurchIndexing::allSet().find(m_churchId);
}

void HolyService::setChurchId(const IdTag& churchId)
{
	if (m_churchId != churchId)
	{
		setDirty();
		m_churchId = churchId;
	}
}

void HolyService::setChurch(const Church* const pChurch)
{
	IdTag id;

	if (pChurch) 
	{
		id = pChurch->getId();
	}
	setChurchId(id);
}

QString HolyService::churchText() const
{
    const Church* const p = getChurch();
	if (p)
	{
		return p->toString();
	}
	return QObject::tr("<none>", "no church");
}

bool HolyService::hasPriest() const
{
	return m_priestId.isValid();
}

Priest * HolyService::getPriest() const
{
    return PriestIndexing::allSet().find(m_priestId);
}

void HolyService::setPriestId(const IdTag& priestId)
{
	if (m_priestId != priestId)
	{
		setDirty();
		m_priestId = priestId;
	}
}

void HolyService::setPriest(const Priest* const pPriest)
{
	IdTag id;

	if (pPriest) 
	{
		id = pPriest->getId();
	}
	setPriestId(id);
}

QString HolyService::priestText() const
{
    const Priest* const p = getPriest();
	if (p)
	{
		return p->toString();
	}
	return QObject::tr("<none>", "no priest");
}

void HolyService::swap(HolyService& other)
{
	qSwap(this->m_intention, other.m_intention);
	qSwap(this->m_startTime, other.m_startTime);
	qSwap(this->m_endTime, other.m_endTime);
	qSwap(this->m_clientId, other.m_clientId);
	qSwap(this->m_priestId, other.m_priestId);
    qSwap(this->m_churchId, other.m_churchId);
//    qSwap(this->m_serviceId, other.m_serviceId);
    qSwap(this->m_timeConstraint, other.m_timeConstraint);
    qSwap(this->m_receivedMoney, other.m_receivedMoney);

	QString tempStr;
	QDateTime tempDt;
	
	tempStr = this->creator(), this->setCreator(other.creator()); other.setCreator(tempStr);
	tempStr = this->lastModificator(), this->setLastModificator(other.lastModificator()); other.setLastModificator(tempStr);
	tempDt = this->created(); this->setCreated(other.created()); other.setCreated(tempDt);
	tempDt = this->lastModified(); this->setLastModified(other.lastModified()); other.setLastModified(tempDt);

	this->setDirty();
	other.setDirty();
}

void HolyService::putCopyInto(HolyService& other) const
{
    other.m_intention = this->m_intention;
    other.m_startTime = this->m_startTime;
    other.m_endTime = this->m_endTime;
    other.m_clientId = this->m_clientId;
    other.m_priestId = this->m_priestId;
    other.m_churchId = this->m_churchId;
    other.m_timeConstraint = this->m_timeConstraint;
    other.m_receivedMoney = this->m_receivedMoney;

    other.setCreator(this->creator());
    other.setLastModificator(this->lastModificator());
    other.setCreated(this->created());
    other.setLastModified(this->lastModified());
}

int HolyService::lengthSeconds() const
{
	return m_startTime.secsTo(m_endTime);
}

const IdTag& HolyService::getId() const
{
    return m_serviceId.getId();
}

void HolyService::setId(const IdTag& newId)
{
    m_serviceId.setId(newId);
}

HolyService::TimeConstraint HolyService::timeConstraint() const
{
    return m_timeConstraint;
}

void HolyService::setTimeConstraint(HolyService::TimeConstraint constraint)
{
    if (m_timeConstraint == constraint)
        return;

    m_timeConstraint = constraint;
    setDirty();
}

double HolyService::receivedMoney() const
{
    return m_receivedMoney;
}

void HolyService::setReceivedMoney(double newValue)
{
    if (std::abs(m_receivedMoney - newValue) < 1e-5)
        return;

    m_receivedMoney = newValue;
    setDirty();
}

bool HolyService::isDirty() const
{
    return m_modificationAware.isDirty();
}

void HolyService::setDirty()
{
    m_modificationAware.setDirty();
}

void HolyService::setClean()
{
    m_modificationAware.setClean();
}
