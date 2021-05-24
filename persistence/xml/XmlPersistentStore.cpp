/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "XmlPersistentStore.h"

#include "HolyServiceStorage.h"
#include "PriestStorage.h"
#include "ClientStorage.h"
#include "ChurchStorage.h"
#include "HolyService.h"
#include "Client.h"
#include "Priest.h"
#include "Church.h"
#include "DomStorageQueue.h"

#include <QtDebug>
#include <QTextStream>
#include <QDomNode>

XmlPersistentStore::XmlPersistentStore(void) 
{
	m_storageQueue = new DomStorageQueue;
}

XmlPersistentStore::~XmlPersistentStore(void) 
{
	delete m_storageQueue;
}

bool XmlPersistentStore::init(const QString& str)
{
	m_filePath = str;
	return m_storageQueue->init(str);
}

QString XmlPersistentStore::identification() const
{
	return m_filePath;
}

bool XmlPersistentStore::storePriest(Priest *p)
{
	return storeObject<PriestStorage, Priest>(p, &XmlPersistentStore::storePriest);
}

bool XmlPersistentStore::storeClient(Client *p)
{
	return storeObject<ClientStorage, Client>(p, &XmlPersistentStore::storeClient);
}

bool XmlPersistentStore::storeChurch(Church *p)
{
	return storeObject<ChurchStorage, Church>(p, &XmlPersistentStore::storeChurch);
}

bool XmlPersistentStore::storeHolyService(HolyService *p)
{
	// history data record
	if (! p->created().isValid())
	{
		p->createNow();
	}
	if (p->isDirty())
	{
		p->modifyNow();
	}
	
	return storeObject<HolyServiceStorage, HolyService>(p, &XmlPersistentStore::storeHolyService);
}

bool XmlPersistentStore::storeHolyService(QDomNode *pNode, IIdentifiable* pIdentifiable)
{
	m_storageQueue->enqueue(DomStorageQueue::HOLY_SERVICE, 
		DomStorageQueue::STORE,
		pNode, pIdentifiable);

	return true;
}

bool XmlPersistentStore::storePriest(QDomNode *pNode, IIdentifiable* pIdentifiable)
{
	m_storageQueue->enqueue(DomStorageQueue::PRIEST, 
		DomStorageQueue::STORE,
		pNode, pIdentifiable);

	return true;
}

bool XmlPersistentStore::storeClient(QDomNode* pNode, IIdentifiable* pIdentifiable)
{
	m_storageQueue->enqueue(DomStorageQueue::CLIENT, 
		DomStorageQueue::STORE,
		pNode, pIdentifiable);

	return true;
}

bool XmlPersistentStore::storeChurch(QDomNode* pNode, IIdentifiable* pIdentifiable)
{
	m_storageQueue->enqueue(DomStorageQueue::CHURCH, 
		DomStorageQueue::STORE,
		pNode, pIdentifiable);

	return true;
}

bool XmlPersistentStore::loadAllHolyServices(tHolyServiceList& services)
{
	return loadAll<HolyServiceStorage, HolyService>(services, 
		&DomStorageQueue::firstServiceNode, &DomStorageQueue::nextServiceNode);
}

bool XmlPersistentStore::loadAllPriests(tPriestList& priests)
{
	return loadAll<PriestStorage, Priest>(priests, 
		&DomStorageQueue::firstPriestNode, &DomStorageQueue::nextPriestNode);
}

bool XmlPersistentStore::loadAllClients(tClientList& clients)
{
	return loadAll<ClientStorage, Client>(clients, 
		&DomStorageQueue::firstClientNode, &DomStorageQueue::nextClientNode);
}

bool XmlPersistentStore::loadAllChurches(tChurchList& churches)
{
	return loadAll<ChurchStorage, Church>(churches, 
		&DomStorageQueue::firstChurchNode, &DomStorageQueue::nextChurchNode);
}

bool XmlPersistentStore::deletePriest(Priest* pPriest)
{
	m_storageQueue->enqueue(DomStorageQueue::PRIEST, 
		DomStorageQueue::DELETE,
		nullptr, pPriest);

	return true;
}

bool XmlPersistentStore::deleteHolyService(HolyService* pService)
{
	m_storageQueue->enqueue(DomStorageQueue::HOLY_SERVICE, 
		DomStorageQueue::DELETE,
		nullptr, pService);

	return true;
}

bool XmlPersistentStore::deleteChurch(Church* pChurch)
{
	m_storageQueue->enqueue(DomStorageQueue::CHURCH, 
		DomStorageQueue::DELETE,
		nullptr, pChurch);

	return true;
}

bool XmlPersistentStore::deleteClient(Client *pClient)
{
	m_storageQueue->enqueue(DomStorageQueue::CLIENT, 
		DomStorageQueue::DELETE,
		nullptr, pClient);

	return true;
}

void XmlPersistentStore::close()
{
	m_storageQueue->saveImmediately(true);
}
