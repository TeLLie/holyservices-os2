/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "BasicModificationAware.h"

BasicModificationAware::BasicModificationAware(void)
{
	setDirty(); // was not stored, yet
}

BasicModificationAware::~BasicModificationAware(void)
{
}

bool BasicModificationAware::isDirty() const
{
	return m_dirty;
}

void BasicModificationAware::setDirty()
{
	m_dirty = true;
}

void BasicModificationAware::setClean()
{
	m_dirty = false;
}
