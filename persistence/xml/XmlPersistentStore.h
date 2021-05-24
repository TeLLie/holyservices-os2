#pragma once

///\class XmlPersistentStore
/// The implementation of IPersistentStore that saves the app data 
/// in a XML document. The saving of document is threaded to avoid
/// blocked GUI when saving. The saving is done often in order
/// to prevent data loss.
///\author Jan 'Kovis' Struhar
///\date 9-2006

#include "IPersistentStore.h"
#include <QDomNode>

class DomStorageQueue;
class IIdentifiable;

class XmlPersistentStore :	public IPersistentStore
{
public:
    bool init(const QString&) override;
    QString identification() const override;

    bool storePriest(Priest *) override;
    bool storeClient(Client *) override;
    bool storeChurch(Church *) override;
    bool storeHolyService(HolyService *) override;

    bool loadAllHolyServices(tHolyServiceList&) override;
    bool loadAllPriests(tPriestList&) override;
    bool loadAllClients(tClientList&) override;
    bool loadAllChurches(tChurchList&) override;

    bool deleteClient(Client *) override;
    bool deletePriest(Priest *) override;
    bool deleteHolyService(HolyService *) override;
    bool deleteChurch(Church *) override;

    void close() override;

private:
    DomStorageQueue *m_storageQueue;
	QString m_filePath;

    XmlPersistentStore(void);
    virtual ~XmlPersistentStore(void);

    typedef bool (XmlPersistentStore::*tStoreFunc)(QDomNode *, IIdentifiable*);

	virtual bool storePriest(QDomNode *, IIdentifiable*);
	virtual bool storeClient(QDomNode *, IIdentifiable*);
	virtual bool storeChurch(QDomNode *, IIdentifiable*);
	virtual bool storeHolyService(QDomNode *, IIdentifiable*);

	template <typename StorageClass, typename StoredClass>
		bool storeObject(StoredClass *p, tStoreFunc f)
	{
		StorageClass storage;

		storage.setStore(this);
		storage.setToBeStored(p);

		return (this->*f)(storage.toXmlDom(), p);
	}

    typedef QDomNode (DomStorageQueue::*tFirstNodeFunc)();
    typedef QDomNode (DomStorageQueue::*tNextNodeFunc)(const QDomNode&);

	template <typename StorageClass, typename StoredClass>
		bool loadAll(QList<StoredClass *>& result, 
			tFirstNodeFunc firstNodeF, tNextNodeFunc nextNodeF)
	{
		result.clear();

		QDomNode aNode = (m_storageQueue->*firstNodeF)();
		if (! aNode.isNull())
		{
			StorageClass storage;
			StoredClass *pStored;

			do 
			{
				storage.fromXmlDom(&aNode);
				if (storage.getLoaded(pStored))
				{
					result.append(pStored);
				}

				aNode = (m_storageQueue->*nextNodeF)(aNode);
			}
			while (! aNode.isNull());
		}
		return true;
	}

    friend class PersistentStoreFactory;
};
