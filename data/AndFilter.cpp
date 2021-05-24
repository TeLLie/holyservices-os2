/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "AndFilter.h"

AndFilter::AndFilter()
{
}

AndFilter::~AndFilter()
{
	qDeleteAll(m_subFilters);
	m_subFilters.clear();
}

void AndFilter::add(IServiceFilter *subFilter)
{
	Q_ASSERT(subFilter);
	if (subFilter)
	{
		m_subFilters.push_back(subFilter);
	}
}

bool AndFilter::pass(const HolyService* const holyService) const
{
	bool retVal = true;

	foreach(IServiceFilter* p, m_subFilters)
	{
		retVal = retVal && p->pass(holyService);
	}
	return retVal;
}


