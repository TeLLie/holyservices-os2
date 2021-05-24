/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "CommonChristianHolidays.h"

#include <QtDebug>
#include <QStringList>
#include <QCoreApplication>

CommonChristianHolidays::CommonChristianHolidays()
{
    initEasterRelatedHolidays();
}

CommonChristianHolidays::~CommonChristianHolidays()
{
}

bool CommonChristianHolidays::isHoliday(const QDate& date, QStringList *description) const
{
    bool retVal = false;

    if (m_easterRelatedHolidays.contains(date))
    {
        if (description)
        {
            description->append(m_easterRelatedHolidays.value(date));
        }
        retVal = true;
    }

    const int day = date.day();

    switch (date.month())
    {
    case 12:
        switch (day)
        {
        case 24:
            if (description)
            {
                description->append(QCoreApplication::translate("CommonChristianHolidays", "Christmas Eve"));
            }
            retVal = true;
            break;
            case 25:
            if (description)
            {
                description->append(QCoreApplication::translate("CommonChristianHolidays", "Christmas Day"));
            }
            retVal = true;
            break;
            case 26:
            if (description)
            {
                description->append(QCoreApplication::translate("CommonChristianHolidays", "St. Stephan"));
            }
            retVal = true;
            break;
        }
    }
    return retVal;
}

void CommonChristianHolidays::initEasterRelatedHolidays()
{
    for (int year = 1990; year <= 2050; ++year)
    {
        int easterDay, easterMonth;

        if (gregorianEasterSunday(year, easterDay, easterMonth))
        {
            const QDate easterSunday(year, easterMonth, easterDay);
            Q_ASSERT(easterSunday.isValid());

//            qDebug() << "Easter is " << easterSunday;

            m_easterRelatedHolidays[easterSunday.addDays(-3)] =
                    QCoreApplication::translate("CommonChristianHolidays", "Maundy Thursday");
            m_easterRelatedHolidays[easterSunday.addDays(-2)] =
                    QCoreApplication::translate("CommonChristianHolidays", "Good Friday");
            m_easterRelatedHolidays[easterSunday.addDays(-1)] =
                    QCoreApplication::translate("CommonChristianHolidays", "Holy Saturday");
            m_easterRelatedHolidays[easterSunday] =
                    QCoreApplication::translate("CommonChristianHolidays", "Easter Sunday");
            m_easterRelatedHolidays[easterSunday.addDays(39)] =
                    QCoreApplication::translate("CommonChristianHolidays", "Ascension Day");
            m_easterRelatedHolidays[easterSunday.addDays(49)] =
                    QCoreApplication::translate("CommonChristianHolidays", "Whitsuntide");
        }
    }
}

bool CommonChristianHolidays::gregorianEasterSunday(const int year, int& day, int& month)
{
    day = month = 0;
    if ((year < 1583) || (year > 4999)) return false;

    int FirstDig, Remain19, temp;    // intermediate results
    int tA, tB, tC, tD, tE;          // table A to E results

    FirstDig = year / 100;        // first 2 digits of year
    Remain19 = year % 19;         // remainder of year / 19

    // calculate PFM date
    temp = ((FirstDig - 15) / 2) + 202 - (11 * Remain19);

    switch (FirstDig)
    {
    case 21:
    case 24:
    case 25:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 34:
    case 35:
    case 38:
        temp -= 1;
        break;

    case 33:
    case 36:
    case 37:
    case 39:
    case 40:
        temp -= 2;
    }

    temp %= 30;

    tA = temp + 21;
    if (temp == 29)
    {
        tA -= 1;
    }
    if ((temp == 28) && (Remain19 > 10))
    {
        tA -= 1;
    }

    // find the next Sunday
    tB = (tA - 19) % 7;

    tC = (40 - FirstDig) % 4;
    if (tC == 3)
    {
        ++tC;
    }

    if (tC > 1)
    {
        ++tC;
    }

    temp = year % 100;
    tD = (temp + (temp / 4)) % 7;

    tE = ((20 - tB - tC - tD) % 7) + 1;
    day = tA + tE;

    // return the date
    if (day > 31)
    {
        day -= 31;
        month = 4;
    }
    else
    {
        month = 3;
    }

    return true;
}
