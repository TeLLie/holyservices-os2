/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "Identifiable.h"

#include <assert.h>

Identifiable::Identifiable(void)
{
	m_id.clear();
}

Identifiable::~Identifiable(void)
{
}

const IdTag& Identifiable::getId() const
{
	return m_id;
}

void Identifiable::setId(const IdTag& id)
{
	assert(id.isValid());

	m_id = id;
}
