/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "HolidayComposition.h"

#include "LocalHolidays.h"
#include "CommonChristianHolidays.h"

#include <QLocale>
#include <QDir>
#include <QtDebug>

HolidayComposition::HolidayComposition(const QString &sharedAppFolder, const QString &userWritableDataFolder)
{
    m_christianHolidays = new CommonChristianHolidays;
    QString holidayFileName;
    m_localReadOnlyHolidays = new LocalHolidays;
    holidayFileName = localHolidaysFileName(sharedAppFolder);
    m_localReadOnlyHolidays->loadFile(holidayFileName);

    m_localWritableHolidays = new LocalHolidays;
    holidayFileName = userSpecificHolidaysFileName(userWritableDataFolder);
    m_localWritableHolidays->loadFile(holidayFileName);
}

HolidayComposition::~HolidayComposition()
{
    delete m_christianHolidays;
    delete m_localReadOnlyHolidays;
    delete m_localWritableHolidays;
}

bool HolidayComposition::isHoliday(const QDate& date, QStringList *description) const
{
    bool retVal = false;
    retVal = m_christianHolidays->isHoliday(date, description) || retVal;
    retVal = m_localReadOnlyHolidays->isHoliday(date, description) || retVal;
    retVal = m_localWritableHolidays->isHoliday(date, description) || retVal;

    return retVal;
}

QString HolidayComposition::localHolidaysFileName(const QString& sharedAppFolder)
{
    const QString localeName = QLocale().name();

    // and load corresponding holiday file
    QDir holidayDir(sharedAppFolder);

    if (holidayDir.cd("holidays"))
    {
        return holidayDir.filePath(localeName + ".txt");
    }
    return QString();
}

QString HolidayComposition::userSpecificHolidaysFileName(const QString &userWritableDataFolder)
{
    const QString localeName = QLocale().name();

    // and load corresponding holiday file
    QDir holidayDir(userWritableDataFolder);
    const QString localeHolidayFile = QString("HolyServicesHolidays") +
            QDir::separator() + localeName + ".txt";
    return holidayDir.filePath(localeHolidayFile);
}

void HolidayComposition::refresh()
{
    m_localReadOnlyHolidays->refresh();
}

