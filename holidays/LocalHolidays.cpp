/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "LocalHolidays.h"

#include "HolidayStorage.h"

#include <QStringList>

struct LocalHolidays::LocalHolidaysImpl
{
    bool isHoliday(const QDate& date, QStringList *description) const
    {
        bool found = false;
        foreach(const HolidayRecord& rec, m_data)
        {
            if (rec.date() == date)
            {
                found = true;
                if (description)
                {
                    description->append(rec.description());
                }
            }
        }
        return found;
    }

    void loadFile(const QString& holidayFileName)
    {
        if (HolidayStorage::loadFromFile(holidayFileName, m_data))
        {
            m_dataSource = holidayFileName;
        }
    }

    void refresh()
    {
        if (! m_dataSource.isEmpty())
        {
            HolidayStorage::loadFromFile(m_dataSource, m_data);
        }
    }

    QList<HolidayRecord> m_data;
    QString m_dataSource;
};

LocalHolidays::LocalHolidays()
{
    pImpl = new LocalHolidaysImpl;
}

LocalHolidays::~LocalHolidays()
{
    delete pImpl;
}

bool LocalHolidays::isHoliday(const QDate& date, QStringList *description) const
{
    return pImpl->isHoliday(date, description);
}

void LocalHolidays::loadFile(const QString& holidayFileName)
{
    pImpl->loadFile(holidayFileName);
}

void LocalHolidays::refresh()
{
    pImpl->refresh();
}

