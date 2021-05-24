/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "PersistentStoreFactory.h"

#include "xml/XmlPersistentStore.h"

#include <QObject>
#include <QString>
#include <assert.h>

PersistentStoreFactory::PersistentStoreFactory(void) {}

PersistentStoreFactory::~PersistentStoreFactory(void) {}

IPersistentStore * PersistentStoreFactory::createPersistentStore(const EStorageType storageType, 
	const QString& initializationString, QString* errMsg)
{
	IPersistentStore *retVal = nullptr;

	switch(storageType)
	{
	case XML_DOM_STORAGE:
		retVal = new XmlPersistentStore();
		if (retVal->init(initializationString) == false)
		{
			destroyPersistentStore(retVal);
			retVal = nullptr;
			if (errMsg)
			{
				*errMsg = QObject::tr("Initialization of XML storage has failed.");
			}
		}
	break;
	default:
		assert(0);

		if (errMsg)
		{
			*errMsg = QObject::tr("Unsupported persistent storage type.");
		}
		break;
	}
	return retVal;
}

void PersistentStoreFactory::destroyPersistentStore(IPersistentStore *pStore)
{
	if (pStore)
	{
		delete pStore;
	}
}
