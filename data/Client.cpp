/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "Client.h"

#include <assert.h>

Client::Client()
{
}

Client::~Client()
{
}

const QString& Client::street() const
{
	return m_street;
}

void Client::setStreet(const QString& street)
{
	if (street != m_street)
	{
		setDirty();
		m_street = street;
	}
}

const QString& Client::contact() const
{
    return m_contact;
}

void Client::setContact(const QString& val)
{
    if (val != m_contact)
    {
        setDirty();
        m_contact = val;
    }
}

QString Client::toString(const TextFormat tf) const
{
	static const QString plainDelimiter("_");
	QString retVal;

	switch (tf)
	{
	case NameSurnameStreet:
		retVal = QString("%1 %2, %3").arg(firstName(), surname(), street());
		break;
	case SurnameStreet:
		retVal = QString("%1, %2").arg(surname(), street());
		break;
	case NameStreet:
                retVal = firstName().trimmed();

                if (! street().trimmed().isEmpty())
                {
                    retVal += ", " + street();
                }
                if (! contact().trimmed().isEmpty())
                {
                    retVal += ", " + contact();
                }
		break;
        case SurnameNameStreetPlain:
                retVal = surname() + plainDelimiter + firstName() + plainDelimiter + m_street;
                break;
        case SurnameNameStreet:
                retVal = QString("%1 %2, %3").arg(surname(), firstName(), street());
                break;
        case SurnameName:
            return QString("%2 %1").arg(firstName(), surname()).trimmed();
        default:
            assert(0);
	}
	return retVal;
}
