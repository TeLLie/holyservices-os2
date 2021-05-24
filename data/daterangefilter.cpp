/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "daterangefilter.h"

#include "HolyService.h"

DateRangeFilter::DateRangeFilter() {}

DateRangeFilter::~DateRangeFilter() {}

void DateRangeFilter::setRange(const QDate &from, const QDate &to)
{
    m_from = qMin(from, to);
    m_to = qMax(from, to);
}

bool DateRangeFilter::pass(const HolyService * const holyService) const
{
    if (holyService == nullptr) return false;
    if (m_from.isNull()) return false;
    if (m_to.isNull()) return false;
    const QDate serviceDate = holyService->startTime().date();

    return (m_from <= serviceDate) && (serviceDate <= m_to);
}
