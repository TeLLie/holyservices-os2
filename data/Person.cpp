/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "Person.h"

#include <QObject>

Person::Person()
{
	setClean();
}

Person::~Person()
{
}

const QString& Person::firstName() const
{
	return m_firstName;
}

void Person::setFirstName(const QString& name)
{
	if (name != m_firstName)
	{
		setDirty();
		m_firstName = name;
	}
}

const QString& Person::surname() const
{
	return m_surname;
}

void Person::setSurname(const QString& name)
{
	if (name != m_surname)
	{
		setDirty();
		m_surname = name;
	}
}

QString Person::toString(Person::TextFormat fmt) const
{
	static const QString plainDelimiter("_");

	switch (fmt)
	{
	case NameSurname:
        return QString("%1 %2").arg(m_firstName, m_surname);
	case SurnameNamePlain:
        return m_surname + plainDelimiter + m_firstName;
    default:
		Q_ASSERT(0);
	}
    return QString();
}

const IdTag& Person::getId() const
{
    return m_id.getId();
}

void Person::setId(const IdTag & idTag)
{
    m_id.setId(idTag);
}
