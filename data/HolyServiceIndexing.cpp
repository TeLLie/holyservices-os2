/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "HolyServiceIndexing.h"

#include <algorithm>
#include <assert.h>

HolyServiceIndexing HolyServiceIndexing::m_allSet;

HolyServiceIndexing& HolyServiceIndexing::allSet()
{
	return m_allSet;
}

void HolyServiceIndexing::releaseAllSet()
{
	allSet().m_data.clear(true);
}

HolyServiceIndexing::HolyServiceIndexing(void)
{
}

HolyServiceIndexing::~HolyServiceIndexing(void)
{
}

tHolyServiceVector HolyServiceIndexing::filter(const tHolyServiceVector& src, 
	const IServiceFilter *const pFilter)
{
	if (pFilter)
	{
		tHolyServiceVector retVal;

		std::for_each(src.begin(), src.end(), 
			addElementIfPassesThroughFilter(retVal, *pFilter));
		return retVal;
	}
	return src;
}

bool HolyServiceIndexing::addElement(HolyService *pService, HolyService*& conflicting)
{
	conflicting = nullptr;
	const bool serviceOk = (pService && 
		(m_data.find(pService->getId()) == nullptr) &&
		(! conflicts(*pService, conflicting)));
	if (serviceOk)
	{ 
		m_data.addElement(pService);
	}
	return serviceOk;
}

void HolyServiceIndexing::addElements(const QList<HolyService *>& newServices)
{
	m_data.addElements(newServices);
}

void HolyServiceIndexing::deleteElement(const HolyService *pToBeDeleted)
{
    // shabby situation solved by const_cast - not nice
    m_data.deleteElement(const_cast<HolyService*>(pToBeDeleted));
}

void HolyServiceIndexing::setData(const tHolyServiceVector& serviceSet)
{
	m_data.setData(serviceSet);
}

tHolyServiceVector HolyServiceIndexing::data() const
{
	return m_data.data();
}

void HolyServiceIndexing::sort()
{
	m_data.sort();
}

tHolyServiceVector HolyServiceIndexing::query(const QDate& from, const QDate& to, 
	const IServiceFilter * const pFilter) const
{
	tHolyServiceVector retVal;
	const tHolyServiceVector my_data = data();
	HolyService tmpService;

	tmpService.setStartTime(StartDateQuery::startOfDate(from));
	tHolyServiceVectorConstIt lowerIt = std::lower_bound(my_data.begin(), my_data.end(), 
		&tmpService, tHolyServiceIndexingContainer::tContainerOrdering());

	tmpService.setStartTime(StartDateQuery::endOfDate(to));
	tHolyServiceVectorConstIt upperIt = std::upper_bound(my_data.begin(), my_data.end(), 
		&tmpService, tHolyServiceIndexingContainer::tContainerOrdering());

	int numberOfItems = std::distance(lowerIt, upperIt);

	if (numberOfItems > 0)
	{
		retVal.resize(numberOfItems);
		qCopy(lowerIt, upperIt, retVal.begin());

		if (pFilter)
		{
			retVal = filter(retVal, pFilter);
		}
	}

	return retVal;
}

bool HolyServiceIndexing::contains(const IdTag& id) const
{
	return (m_data.find(id) != nullptr);
}

bool HolyServiceIndexing::conflicts(const HolyService& serviceToBeAdded, 
		HolyService*& conflicting) const
{
	conflicting = nullptr;

	bool hasConflict = contains(serviceToBeAdded.getId());

	if (! hasConflict)
	{
		tHolyServiceVector timeSlice = query(serviceToBeAdded.startTime().date(), 
			serviceToBeAdded.endTime().date());

		foreach(HolyService *p, timeSlice)
		{
			hasConflict = serviceToBeAdded.conflicts(*p);
			if (hasConflict)
			{
				conflicting = p;
				break;
			}
		}
	}

	return hasConflict;
}

int HolyServiceIndexing::serviceCountAtDate(const QDate& date, 
	const IServiceFilter *const pFilter) const
{
	return query(date, date, pFilter).count();
}

void HolyServiceIndexing::removeClient(Client *pClient)
{
	tHolyServiceVector data = m_data.data();

	std::for_each(data.begin(), data.end(), RemoveClientReference(pClient->getId()));
}

void HolyServiceIndexing::removeChurch(Church *pChurch)
{
	tHolyServiceVector data = m_data.data();

	std::for_each(data.begin(), data.end(), 
		RemoveChurchReference(pChurch->getId()));
}

void HolyServiceIndexing::removePriest(Priest *pPriest)
{
	tHolyServiceVector data = m_data.data();

	std::for_each(data.begin(), data.end(), 
		RemovePriestReference(pPriest->getId()));
}

HolyService* HolyServiceIndexing::find(const IdTag& id) const
{
	return m_data.find(id);
}
