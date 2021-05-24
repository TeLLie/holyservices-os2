#pragma once

class IPersistentStore;

///\class IClassStorage
/// IClassStorage presents helper class for the persistent storage,
/// It should transform the class instances to the form digestible for 
/// the persistent storage a vice versa - comvert the persisted form
/// to the active instance. Assumption for proper functioning is, that
/// persistent store is assigned.
///\par
/// Expected process is for storage: setToBeStored() call followed by some magic 
/// out of this interface. 
/// Reversely some magic out of this interface happens (\sa IXmlTransformer)
/// and on the end new instance is picked up by getLoaded().
///\date 8-2006
///\author Jan 'Kovis' Struhar
//
template <typename T>class IClassStorage
{
public:
	virtual ~IClassStorage(void) {}

	virtual bool setToBeStored(const T* const) = 0;
	
	virtual bool getLoaded(T*& /*out*/) = 0;

	virtual void setStore(IPersistentStore *) = 0;
	virtual IPersistentStore* getStore(void) const = 0;

protected:
	IClassStorage(void)	{}
};
