#pragma once

#include <QDate>
#include "HsTypedefs.h"
#include "../common/GenericIndexing.h"

class IdTag;
class IServiceFilter;

///\class HolyServiceIndexing
///\brief Indexing, sorting and querying of holy services.
/// Global instance allSet owns the services and releases them. New services
/// should be added only via static addElement() to the allset.
///\par Local (not static allset) instances
/// Local instances of CHolyServiceIndexing with contents derived from allset
/// should be initialized via setData() call - if global allset is changed (delete/add)
/// the local instance should be refreshed.
///\par
/// The class is very, very similar to GenericIndexing - but it is not derived,
/// since in allSet() method I need all the additional methods (conflicts()) available
/// That is why this indexing is defined "in terms of" GenericIndexing
///\date 9-2006
class HolyServiceIndexing
{
public:
    HolyServiceIndexing(void);
    virtual ~HolyServiceIndexing(void);

	//@{
	///\name All services holding global structure related
    static HolyServiceIndexing& allSet();
	static void releaseAllSet();

	/// generic filtering of holy services
	///\post preserves the order of services in src
	static tHolyServiceVector filter(const tHolyServiceVector& src, 
        const IServiceFilter* const);

	/// does not add conflicting service
	///\return false if newly added service conflicts, then second
	/// attribute contains the conflicting service
    bool addElement(HolyService*, HolyService*& conflicting);

	/// mass adding does not check conflicts
    void addElements(const QList<HolyService*>&);

    void deleteElement(const HolyService *);
	//@}

	void sort(); ///< sorts by time of service beginning ascending
	
	tHolyServiceVector query(const QDate& from, const QDate& to, 
		const IServiceFilter* const = nullptr) const;

	/// if there is time conflict, method returns true and existing service in conflict
	bool conflicts(const HolyService& serviceToBeAdded, 
		HolyService*& conflicting) const;

	/// returns count of holy services for this day and other criteria (church, priest)
	int serviceCountAtDate(const QDate&, const IServiceFilter *const = nullptr) const;

	/// to initialize other than static allSet() instance
	void setData(const tHolyServiceVector& serviceSet);
	
	/// retrieve copy of inner container with all maintained data 
	/// - for more precise selection use query() method
	///\sa query()
	tHolyServiceVector data() const;

	bool contains(const IdTag& id) const;

    /// the id retrieval just like GenericIndexing has
	HolyService* find(const IdTag& id) const;

	///{@
	///\name Remove items from all services
	/// usually on item delete
	void removeClient(Client *);
	void removeChurch(Church *);
	void removePriest(Priest *);
	//@}

private:
    static HolyServiceIndexing m_allSet;

    typedef GenericIndexing<HolyService*, tHolyServiceVector,
		DateTimeAscendingSorting> tHolyServiceIndexingContainer;
	tHolyServiceIndexingContainer m_data;
};
