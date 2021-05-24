/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ReportColumnDescription.h"

#include "HolyService.h"
#include "Priest.h"
#include "Church.h"
#include "Client.h"
#include "IHolidays.h"

#include <QCoreApplication>
#include <QLocale>

ReportColumnDescription::ReportColumnDescription()
{
	m_columnType = PriestName;
	m_visible = true;
}

ReportColumnDescription::~ReportColumnDescription() {}

const QString timeFormat("hh:mm");

QString ReportColumnDescription::columnData(
        const EType columnType, const HolyService* const hs, const IHolidays *const holidays)
{
    if (hs == nullptr)
    {
        return {};
    }

	static const QLocale myLocale;
    QString retVal;

    switch (columnType)
    {
    case PriestName:
        if (hs->getPriest())
        {
            retVal = hs->getPriest()->toString();
        }
        break;
    case ChurchName:
        if (hs->getChurch())
        {
            retVal = hs->getChurch()->toString();
        }
        break;
    case ChurchAbbreviation:
        if (hs->getChurch())
        {
            retVal = hs->getChurch()->abbreviation();
        }
        break;
    case Intention:
        retVal = hs->intention();
        break;
    case ServiceWeekday:
        retVal = hs->startTime().date().toString("ddd");
        break;
    case ServiceStartTime:
        retVal = myLocale.toString(hs->startTime().time(), timeFormat);
        break;
    case ServiceStartDate:
        retVal = myLocale.toString(hs->startTime().date(), QLocale::ShortFormat);
        break;
    case ServiceStartDateShort:
        retVal = myLocale.toString(hs->startTime().date(), "d. M.");
        break;
    case ServiceStartDateTime:
        retVal = columnData(ServiceStartDate, hs, holidays) + ' ' +
                 columnData(ServiceStartTime, hs, holidays);
        break;
    case ServiceEndTime:
        retVal = myLocale.toString(hs->endTime().time(), timeFormat);
        break;
    case ServiceEndDate:
        retVal = myLocale.toString(hs->endTime().date(), QLocale::ShortFormat);
        break;
    case ServiceEndDateTime:
        retVal = columnData(ServiceEndDate, hs, holidays) + ' ' +
                 columnData(ServiceEndTime, hs, holidays);
        break;
    case ClientName:
        if (hs->getClient())
        {
            // address the ancestor version of toString() method
            retVal = static_cast<Person*>(hs->getClient())->toString(Person::NameSurname);
        }
        break;
    case ClientAddress:
        if (hs->getClient())
        {
            retVal = hs->getClient()->street();
        }
        break;
    case HolidayText:
        if (holidays)
        {
            QStringList holidayText;
            if (holidays->isHoliday(hs->startTime().date(), &holidayText))
            {
                retVal = holidayText.join("\n");
            }
        }
        break;
    case Stipend:
        retVal = QString("%L1").arg(hs->receivedMoney(), 0, 'f', 2);
        break;
    case ColumnCount: // fall through you should never get here
    default:
            Q_ASSERT(0);
    }

    // older Qt like 5.9 are allergic to newlines, null characters and screw the OpenOffice document
    const auto nullPos = retVal.indexOf(QChar::Null);
    if (nullPos >= 0)
    {
        retVal.truncate(nullPos);
    }
    return retVal;
}

QString ReportColumnDescription::columnData(
        const ReportColumnDescription::EType columnType, const QDate& dt, const IHolidays * const holidays)
{
    static const QLocale myLocale;
    switch (columnType)
    {
    case ServiceWeekday: return dt.toString("ddd");
    case ServiceStartDate:
    case ServiceEndDate:
        return myLocale.toString(dt, QLocale::ShortFormat);
    case ServiceStartDateShort:
        return myLocale.toString(dt, "d. M.");
    case HolidayText:
        if (holidays)
        {
            QStringList holidayText;
            if (holidays->isHoliday(dt, &holidayText))
            {
                return holidayText.join("\n");
            }
        }
        break;
    case ColumnCount: // fall through you should never get here
    default: break;
    }
    return QString();
}

QString ReportColumnDescription::columnHeader(EType columnType)
{
	switch (columnType)
	{
    case PriestName: return QCoreApplication::translate("ReportColumnDescription", "Celebrant");
    case ChurchName: return QCoreApplication::translate("ReportColumnDescription", "Church");
    case ChurchAbbreviation:
        return QCoreApplication::translate("ReportColumnDescription", "Church abbreviation");
    case Intention: return QCoreApplication::translate("ReportColumnDescription", "Intention");
    case ServiceWeekday: return QCoreApplication::translate("ReportColumnDescription", "Weekday");
    case ServiceStartDate: return QCoreApplication::translate("ReportColumnDescription", "Date");
    case ServiceStartDateShort: return QCoreApplication::translate("ReportColumnDescription", "Short date");
    case ServiceStartTime:
    case ServiceStartDateTime: return QCoreApplication::translate("ReportColumnDescription", "Start", "Beginning of the service");
    case ServiceEndDate: return QCoreApplication::translate("ReportColumnDescription", "End date");
	case ServiceEndTime: 
    case ServiceEndDateTime: return QCoreApplication::translate("ReportColumnDescription", "End", "End time of the service");
    case ClientName: return QCoreApplication::translate("ReportColumnDescription", "Client");
    case ClientAddress: return QCoreApplication::translate("ReportColumnDescription", "Address", "Client address");
    case HolidayText: return QCoreApplication::translate("ReportColumnDescription", "Holiday");
    case Stipend: return QCoreApplication::translate("ReportColumnDescription", "Stipend", "Service fee");
    // column count will assert, too
	case ColumnCount: 
	default:
        Q_ASSERT(0);
	}
	return QString();
}


void ReportColumnDescription::setColumnType(EType newType)
{
    m_columnType = newType;
}

ReportColumnDescription::EType ReportColumnDescription::columnType() const
{
    return m_columnType;
}

bool ReportColumnDescription::visible() const
{
    return m_visible;
}

void ReportColumnDescription::setVisible(bool newVal)
{
    m_visible = newVal;
}

const QString columnTypeKey("ColumnType");
const QString columnVisibleKey("Visible");

QVariantMap ReportColumnDescription::toVariantMap() const
{
    QVariantMap vMap;

    vMap[columnTypeKey] = m_columnType;
    vMap[columnVisibleKey] = m_visible;

    return vMap;
}

bool ReportColumnDescription::fromVariantMap(const QVariantMap& vMap)
{
    if (vMap.contains(columnTypeKey) && vMap.contains(columnVisibleKey))
    {
        bool conversionOk = false;
        m_visible = vMap.value(columnVisibleKey).toBool();
        m_columnType = static_cast<EType>(vMap.value(columnTypeKey).toInt(&conversionOk));
        return conversionOk;
    }
    return false;
}
