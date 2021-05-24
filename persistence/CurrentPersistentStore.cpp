/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "CurrentPersistentStore.h"

#include <stddef.h>

IPersistentStore * CurrentPersistentStore::m_store = nullptr;

CurrentPersistentStore::CurrentPersistentStore() { }
CurrentPersistentStore::~CurrentPersistentStore() { }

IPersistentStore * CurrentPersistentStore::get()
{
	return m_store;
}

void CurrentPersistentStore::set(IPersistentStore *pStore) 
{
	m_store = pStore;
}
