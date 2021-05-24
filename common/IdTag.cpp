/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "IdTag.h"

#include <assert.h>

IdTag IdTag::createInitialized()
{
	IdTag id;

	id.initId();
	return id;
}

IdTag IdTag::createFromString(const QString& strId)
{
	IdTag id;

	id.setIdFromString(strId);
	return id;
}

IdTag::IdTag(void)
{
}

IdTag::IdTag(const IdTag& orig)
{
	m_id = orig.m_id;
}

IdTag &IdTag::operator=(const IdTag& rhs)
{
    IdTag tmp(rhs);
    tmp.swap(*this);
    return *this;
}

void IdTag::initId()
{
	setIdFromString(QUuid::createUuid().toString());
}

void IdTag::setIdFromString(const QString& newId)
{
	assert(! newId.isEmpty());
	m_id = newId;
}

QString IdTag::toString() const
{
    return m_id.toString();
}

bool IdTag::isValid() const
{
	return (! m_id.isNull());
}

void IdTag::clear(void)
{
	m_id = QUuid();
}

bool IdTag::operator<(const IdTag& rhs) const
{
	return (m_id < rhs.m_id);
}

bool IdTag::operator==(const IdTag& rhs) const
{
	return (m_id == rhs.m_id);
}

bool IdTag::operator!=(const IdTag& rhs) const
{
    return (! (*this == rhs));
}

void IdTag::swap(IdTag &other) noexcept
{
    if (this == &other) return;

    const auto tmpId = other.m_id;
    other.m_id = this->m_id;
    this->m_id = tmpId;
}
