/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ChurchFilter.h"
#include "Church.h"
#include "HolyService.h"

ChurchFilter::ChurchFilter(const IdTag& id) : m_id(id)
{
}

ChurchFilter::~ChurchFilter()
{
}

bool ChurchFilter::pass(const HolyService* const holyService) const
{
	if (holyService)
	{
		const Church* const p = holyService->getChurch();

		return (p) ? (p->getId() == m_id) : false;
	}
	return false;
}


