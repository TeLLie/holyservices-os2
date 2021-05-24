#pragma once

///\class PersistentStoreFactory
/// Factory class for instantiating/destruction of persistent storage,
///\date 9-2006
///\author Jan 'Kovis' Struhar

#include <stddef.h>

class IPersistentStore;
class QString;

class PersistentStoreFactory
{
public:
	enum EStorageType { UNKNOWN, XML_DOM_STORAGE };

	static IPersistentStore * createPersistentStore(const EStorageType storageType, 
		const QString& initializationString, QString* errMsg = nullptr);
	static void destroyPersistentStore(IPersistentStore *);

private:
    PersistentStoreFactory(void);
    virtual ~PersistentStoreFactory(void);
};
