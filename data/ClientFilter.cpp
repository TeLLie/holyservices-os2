/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ClientFilter.h"

#include "HolyService.h"
#include "Client.h"

ClientFilter::ClientFilter(const IdTag& clientId) : m_id(clientId)
{
}

ClientFilter::~ClientFilter()
{
}

bool ClientFilter::pass(const HolyService* const holyService) const
{
	const Client * const pClient = holyService->getClient();

	if (m_id.isValid())
	{
		if (pClient)
		{
			return (m_id == pClient->getId());
		}
	}
	else
	{
		return (pClient == nullptr);
	}
	return false;
}

