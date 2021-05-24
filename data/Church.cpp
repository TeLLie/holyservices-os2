/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "Church.h"

Church::Church() : m_easting(0.0), m_northing(0.0), m_color(87, 171, 255)
{
	setClean();
}

Church::~Church()
{
}

const QString& Church::name() const
{
	return m_name;
}

void Church::setName(const QString& aName)
{
	if (aName != m_name)
	{
		setDirty();
		m_name = aName;
	}
}

const QString& Church::place() const
{
	return m_place;
}

void Church::setPlace(const QString& aPlace)
{
	if (aPlace != m_place)
	{
		setDirty();
		m_place = aPlace;
	}
}

const QString& Church::abbreviation() const
{
    return m_abbreviation;
}

void Church::setAbbreviation(const QString& newAbbreviation)
{
    if (m_abbreviation != newAbbreviation)
    {
        setDirty();
        m_abbreviation = newAbbreviation;
    }
}

QString Church::toString() const
{
	return QString("%1, %2").arg(m_name, m_place);
}

QColor Church::color() const
{
	return m_color;
}

void Church::setColor( const QColor& clr)
{
	if (clr != m_color)
	{
		setDirty();
		m_color = clr;
	}
}

const IdTag & Church::getId() const
{
    return m_myIdHandler.getId();
}

void Church::setId(const IdTag& idTag)
{
    m_myIdHandler.setId(idTag);
}

void Church::setEasting(double anEasting)
{
    if (qAbs(m_easting - anEasting) > 1e-6)
    {
        m_easting = anEasting;
        setDirty();
    }
}

double Church::easting() const
{
    return m_easting;
}

void Church::setNorthing(double aNorthing)
{
    if (qAbs(m_northing - aNorthing) > 1e-6)
    {
        m_northing = aNorthing;
        setDirty();
    }
}

double Church::northing() const
{
    return m_northing;
}

const Church::tServiceTemplateList& Church::usualServices() const
{
	return m_usualServices;
}

void Church::setUsualServices( const Church::tServiceTemplateList& services )
{
	if (m_usualServices != services)
	{
		setDirty();
		m_usualServices = services;
	}
}

int Church::countOfActiveServices() const
{
	int retVal = 0;

	foreach(ServiceTemplate s, m_usualServices)
	{
		if (s.m_active) 
		{
			++retVal;
		}
	}
    return retVal;
}

Qt::DayOfWeek Church::equivalent(ServiceTemplate::ServiceTerm servTerm)
{
    switch (servTerm)
    {
    case ServiceTemplate::Monday: return Qt::Monday;
    case ServiceTemplate::Tuesday: return Qt::Tuesday;
    case ServiceTemplate::Wednesday: return Qt::Wednesday;
    case ServiceTemplate::Thursday: return Qt::Thursday;
    case ServiceTemplate::Friday: return Qt::Friday;
    case ServiceTemplate::Saturday: return Qt::Saturday;
    default:
        return Qt::Sunday;
    }
}

/////////////////////////////////////////////////////////
// inner struct ServiceTemplate
/////////////////////////////////////////////////////////
Church::ServiceTemplate::ServiceTemplate()
{
        m_serviceTerm = Monday;
	m_durationInMinutes = 60;
	m_start = QTime(18, 0);
	m_active = true;
}

bool Church::ServiceTemplate::operator==( const ServiceTemplate& rhs ) const
{
	return (m_active == rhs.m_active) &&
		(m_priest == rhs.m_priest) &&
                (m_serviceTerm == rhs.m_serviceTerm) &&
		(m_start == rhs.m_start) &&
		(m_durationInMinutes == rhs.m_durationInMinutes);
}

Qt::DayOfWeek Church::ServiceTemplate::weekDay() const
{
    switch(m_serviceTerm)
    {
    case Monday: return Qt::Monday;
    case Tuesday: return Qt::Tuesday;
    case Wednesday: return Qt::Wednesday;
    case Thursday: return Qt::Thursday;
    case Friday: return Qt::Friday;
    case Saturday: return Qt::Saturday;
    case Sunday1:
    case Sunday2:
    case Sunday3:
        return Qt::Sunday;
    };
    Q_ASSERT(0);
    return Qt::Monday;
}

