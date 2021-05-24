/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "PriestFilter.h"
#include "Priest.h"
#include "HolyService.h"

PriestFilter::PriestFilter(const IdTag& id) : m_id(id)
{
}

PriestFilter::~PriestFilter()
{
}

bool PriestFilter::pass(const HolyService* const holyService) const
{
	if (holyService)
	{
		const Priest* const p = holyService->getPriest();

		return (p) ? (p->getId() == m_id) : false;
	}
	return false;
}


