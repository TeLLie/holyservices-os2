/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ChurchIndexing.h"

#include "ChurchFilter.h"
#include "HolyServiceIndexing.h"

ChurchIndexing::ChurchIndexing()
{
}

ChurchIndexing::~ChurchIndexing()
{
}

bool ChurchIndexing::hasDuplicities(const super& data)
{
	const tChurchVector dataCopy = data.data();

	return (std::adjacent_find(dataCopy.begin(), dataCopy.end(),
		ChurchValueEquality()) != dataCopy.end());
}

tChurchVector ChurchIndexing::findDuplicities(super& data, bool leaveOneSurvivor)
{
	tChurchVector retVal;
	ChurchValueEquality equality;
	const tChurchVector dataCopy = data.data();

	// find first duplicity
	tChurchVector::const_iterator duplicityStartIt = 
		std::adjacent_find(dataCopy.begin(), dataCopy.end(),
		equality);

	if (duplicityStartIt != dataCopy.end())
	{
		do 
		{
			// browse through equal_range and copy unused churches to retVal
			tChurchVector::const_iterator it = duplicityStartIt;
			bool allFromRangeToBeDeleted = true;

			for (; it != dataCopy.end(); ++it)
			{
				const ChurchFilter churchFilter((*it)->getId());
	
				if (equality(*duplicityStartIt, *it))
				{
					if (HolyServiceIndexing::filter(
						HolyServiceIndexing::allSet().data(), &churchFilter).isEmpty())
					{
						retVal.append(*it);
					}
					else
					{
						allFromRangeToBeDeleted = false;
					}
				}
				else 
				{
					break;
				}
			}

			if (leaveOneSurvivor && allFromRangeToBeDeleted)
			{
				retVal.remove(retVal.size() - 1);
			}

			// find next duplicity after end of inspected duplicity-range
			if (it != dataCopy.end())
			{
				duplicityStartIt = std::adjacent_find(it, dataCopy.end(),
					equality);
			}
			else // end the loop
			{
				duplicityStartIt = dataCopy.end();
			}
		// until end() is returned
		}
		while (duplicityStartIt != dataCopy.end());
	}

	return retVal;
}

