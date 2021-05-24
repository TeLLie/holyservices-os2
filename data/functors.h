#ifndef FUNCTORS_H_INCLUDED_F0624082_A0E7_49DF_9B64_8FF94FB834A1
#define FUNCTORS_H_INCLUDED_F0624082_A0E7_49DF_9B64_8FF94FB834A1

///\file functors.h
/// The file contains STL-functor classes of various purpose - 
/// sorting, filtering, predicates a.s.o.

#include "HolyService.h"
#include "Person.h"
#include "Client.h"
#include "Priest.h"
#include "Church.h"
#include "IServiceFilter.h"
#include "IPersistentStore.h"
#include "HsTypedefs.h"

#include <functional>
#include <assert.h>

struct DateTimeAscendingSorting : 
	public std::binary_function<const HolyService * const, const HolyService * const, bool>
{
	bool operator()(const HolyService * const p1, const HolyService * const p2)
	{
		if (p1 && p2)
		{
			return p1->startTime() < p2->startTime();
		}
		else if (p1 == nullptr)
		{
			return true;
		}
		else if (p2 == nullptr)
		{
			return false;
		}
		return false;
	}
};

struct PersonSurnameAscendingSorting : 
	public std::binary_function<const Person * const, const Person * const, bool>
{
	bool operator()(const Person * const p1, const Person * const p2)
	{
		if (p1 && p2)
		{
			return (QString::localeAwareCompare(
				p1->toString(Person::SurnameNamePlain), 
				p2->toString(Person::SurnameNamePlain)) < 0);
		}
		else if (p1 == nullptr)
		{
			return true;
		}
		else if (p2 == nullptr)
		{
			return false;
		}
		return false;
	}
};

struct ClientSurnameAscendingSorting : 
	public std::binary_function<const Client * const, const Client * const, bool>
{
	bool operator()(const Client * const p1, const Client * const p2)
	{
		if (p1 && p2)
		{
			return (QString::localeAwareCompare(
				p1->toString(Client::SurnameNameStreetPlain), 
				p2->toString(Client::SurnameNameStreetPlain)) < 0);
		}
		else if (p1 == nullptr)
		{
			return true;
		}
		else if (p2 == nullptr)
		{
			return false;
		}
		return false;
	}
};

struct PlaceNameSorting : 
	public std::binary_function<const Church * const, const Church * const, bool>
{
	bool operator()(const Church * const p1, const Church * const p2)
	{
		if (p1 && p2)
		{
			return (QString::localeAwareCompare(p1->place(), p2->place()) < 0) ||
				((p1->place() == p2->place()) && 
				 (QString::localeAwareCompare(p1->name(), p2->name()) < 0));
		}
		else if (p1 == nullptr)
		{
			return true;
		}
		else if (p2 == nullptr)
		{
			return false;
		}
		return false;
	}
};

class IdEquality : public std::unary_function<const IIdentifiable *const, bool>
{
public:
	IdEquality(const IdTag& soughtFor) : m_soughtId(soughtFor) {};
	bool operator()(const IIdentifiable *const p)
	{
		return (p && p->getId() == m_soughtId);
	}
private:
	const IdTag& m_soughtId;
	IdEquality& operator=(const IdEquality&);
};

class StartDateQuery : public std::unary_function<HolyService *, void>
{
public:
	static QDateTime startOfDate(const QDate& date)
	{
		return QDateTime(date, QTime(0,0,0));
	}

	static QDateTime endOfDate(const QDate& date)
	{
		return QDateTime(date, QTime(23,59,59));
	}

	StartDateQuery(const QDate& queryStart, const QDate& queryEnd, tHolyServiceVector& result) :
	  m_result(result)
	  {
		  m_result.clear();

		  m_start = startOfDate(queryStart);
		  m_end = endOfDate(queryEnd);
	  }
	  
	  void operator()(HolyService* p)
	  {
		  const QDateTime serviceStart = p->startTime();
		  if ((serviceStart >= m_start) && (serviceStart < m_end))
		  {
			  m_result.append(p);
		  }
	  }
private:
	QDateTime m_start, m_end;
	tHolyServiceVector& m_result;

	StartDateQuery& operator=(const StartDateQuery&);
};

class addElementIfPassesThroughFilter : public std::unary_function<HolyService *, void>
{
public:
	addElementIfPassesThroughFilter(tHolyServiceVector& passed, const IServiceFilter& filter) :
	  m_passed(passed), m_filter(filter)
	{
		m_passed.clear();
	}

	void operator()(HolyService *pService)
	{
		if (m_filter.pass(pService))
		{
			m_passed.append(pService);
		}
	}
private:
	/// vector accumulates all passed services
	tHolyServiceVector& m_passed; 
	const IServiceFilter& m_filter;

	addElementIfPassesThroughFilter& operator=(const addElementIfPassesThroughFilter&);
};

template <typename T> class SaveModifiedBase : public std::unary_function<T*, void>
{
public:
	SaveModifiedBase(IPersistentStore *pStore) : m_persistentStore(pStore) {}
	virtual ~SaveModifiedBase() {}
	virtual void operator()(T* p)
	{
		saveDirtyInstance(p);
	}
protected:
	IPersistentStore *m_persistentStore;

	virtual void saveDirtyInstance(T*) = 0;
};

class SaveModifiedService : public SaveModifiedBase<HolyService>
{
	typedef SaveModifiedBase<HolyService> super;
public:
	SaveModifiedService(IPersistentStore *pStore) : super(pStore) {}

protected:
	virtual void saveDirtyInstance(HolyService *pService)
	{
		if (pService->isDirty())
		{
			const bool storeOk = m_persistentStore->storeHolyService(pService);
			assert(storeOk);

			if (storeOk)
			{
				pService->setClean();
			}
		}
	}
};

class SaveModifiedClient : public SaveModifiedBase<Client>
{
	typedef SaveModifiedBase<Client> super;
public:
	SaveModifiedClient(IPersistentStore *pStore) : super(pStore) {}

protected:
	virtual void saveDirtyInstance(Client *pClient)
	{
		if (pClient->isDirty())
		{
			const bool storeOk = m_persistentStore->storeClient(pClient);
			assert(storeOk);

			if (storeOk)
			{
				pClient->setClean();
			}
		}
	}
};

class SaveModifiedPriest : public SaveModifiedBase<Priest>
{
	typedef SaveModifiedBase<Priest> super;
public:
	SaveModifiedPriest(IPersistentStore *pStore) : super(pStore) {}

protected:
	virtual void saveDirtyInstance(Priest *pPriest)
	{
		if (pPriest->isDirty())
		{
			const bool storeOk = m_persistentStore->storePriest(pPriest);
			assert(storeOk);

			if (storeOk)
			{
				pPriest->setClean();
			}
		}
	}
};

class SaveModifiedChurch : public SaveModifiedBase<Church>
{
	typedef SaveModifiedBase<Church> super;
public:
	SaveModifiedChurch(IPersistentStore *pStore) : super(pStore) {}

protected:
	virtual void saveDirtyInstance(Church *pChurch)
	{
		if (pChurch->isDirty())
		{
			const bool storeOk = m_persistentStore->storeChurch(pChurch);
			assert(storeOk);

			if (storeOk)
			{
				pChurch->setClean();
			}
		}
	}
};

class RemoveClientReference : public std::unary_function<HolyService*, void>
{
public:
	RemoveClientReference(const IdTag& clientId) : m_clientId(clientId) {}
	virtual ~RemoveClientReference() {}
	virtual void operator()(HolyService* p)
	{
		if (p && p->getClient() && (p->getClient()->getId() == m_clientId))
		{
			p->setClientId(IdTag());
		}
	}

private:
	const IdTag m_clientId;
	RemoveClientReference& operator=(const RemoveClientReference&);
};

class RemovePriestReference : public std::unary_function<HolyService*, void>
{
public:
	RemovePriestReference(const IdTag& priestId) : m_priestId(priestId) {}
	virtual ~RemovePriestReference() {}
	virtual void operator()(HolyService* p)
	{
		if (p && p->getPriest() && (p->getPriest()->getId() == m_priestId))
		{
			p->setPriestId(IdTag());
		}
	}

private:
	const IdTag m_priestId;
	RemovePriestReference& operator=(const RemovePriestReference&);
};

class RemoveChurchReference : public std::unary_function<HolyService*, void>
{
public:
	RemoveChurchReference(const IdTag& churchId) : m_churchId(churchId) {}
	virtual ~RemoveChurchReference() {}
	virtual void operator()(HolyService* p)
	{
		if (p && p->getChurch() && (p->getChurch()->getId() == m_churchId))
		{
			p->setChurchId(IdTag());
		}
	}

private:
	const IdTag m_churchId;
	RemoveChurchReference& operator=(const RemoveChurchReference&);
};

//@{
/// use std::adjacent_find to look for duplicities - case insensitive
struct ChurchValueEquality : 
	public std::binary_function<const Church* const, const Church* const, bool>
{
	bool operator()(const Church* const p1, const Church* const p2)
	{
		if (p1 && p2 && 
			((p1 == p2) || (p1->toString().toLower() == p2->toString().toLower())) 
			)
		{
			return true;
		}
		return false;
	}
};

struct ClientValueEquality : 
	public std::binary_function<const Client* const, const Client* const, bool>
{
	bool operator()(const Client* const p1, const Client* const p2)
	{
		if (p1 && p2 && 
			((p1 == p2) || 
			(p1->toString(Client::SurnameNameStreetPlain).toLower() == 
			p2->toString(Client::SurnameNameStreetPlain).toLower())) 
			)
		{
			return true;
		}
		return false;
	}
};

struct PriestValueEquality : 
	public std::binary_function<const Priest* const, const Priest* const, bool>
{
	bool operator()(const Priest* const p1, const Priest* const p2)
	{
		if (p1 && p2 && 
			((p1 == p2) || 
			(p1->toString(Person::SurnameNamePlain).toLower() == 
			p2->toString(Person::SurnameNamePlain).toLower())) 
			)
		{
			return true;
		}
		return false;
	}
};

//@}

#endif

