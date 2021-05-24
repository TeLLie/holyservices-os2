/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "AssignedFilter.h"

#include "HolyService.h"
#include "UserSettings.h"

bool AssignedFilter::pass(const HolyService* const holyService) const
{
    return (holyService && isServiceAssigned(*holyService));
}

bool AssignedFilter::isServiceAssigned(const HolyService& holyService)
{
    if (UserSettings::get().assignedCriteria() == UserSettings::IntentionIsFilled)
    {
        return holyService.hasIntention();
    }
    return holyService.hasClient();
}

