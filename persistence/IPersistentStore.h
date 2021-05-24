#pragma once

///\class IPersistentStore
/// Basic interface for persisting of application data.
/// Usual workflow is that you instantiate the persistent store
/// via services of CPersistentStoreFactory, call init() method
/// and load/store/delete the class instances. As you can see
/// there is no support for querying objects in the persistent storage
/// the assumed data amount fits very well into memory.
/// At the moment the only implementation of this interface is
/// a XML file, but the choice of persistence platform is up to you.
/// SQL database can be used as well.
///\date 8-2006
///\author Jan 'Kovis' Struhar

#include <QList>

class HolyService;
class Priest;
class Church;
class Client;

class QString;

class IPersistentStore
{
public:
	typedef QList<HolyService *> tHolyServiceList;
	typedef QList<Priest *> tPriestList;
	typedef QList<Client *> tClientList;
	typedef QList<Church *> tChurchList;

	virtual ~IPersistentStore(void)	{}

	/// initialize the store before you start operation
	/// be the parameter file name or DB connection string
	virtual bool init(const QString&) = 0;

	/// file path for file storages, Db name or connect string 
	/// for database implementation
	virtual QString identification() const = 0;

	virtual bool storePriest(Priest *) = 0;
	virtual bool storeClient(Client *) = 0;
	virtual bool storeChurch(Church *) = 0;
	virtual bool storeHolyService(HolyService *) = 0;

	virtual bool loadAllHolyServices(tHolyServiceList&) = 0;
	virtual bool loadAllPriests(tPriestList&) = 0;
	virtual bool loadAllClients(tClientList&) = 0;
	virtual bool loadAllChurches(tChurchList&) = 0;

	virtual bool deleteClient(Client *) = 0;
	virtual bool deletePriest(Priest *) = 0;
	virtual bool deleteHolyService(HolyService *) = 0;
	virtual bool deleteChurch(Church *) = 0;

	/// final cleanup before the end 
	virtual void close() = 0;

protected:
	IPersistentStore(void)	{}
};

