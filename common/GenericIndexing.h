#ifndef GENERICINDEXING_H_INCLUDED_B93426F1_6CEC_484A_8EEB_E3D622EFE79C
#define GENERICINDEXING_H_INCLUDED_B93426F1_6CEC_484A_8EEB_E3D622EFE79C

///\class GenericIndexing
///
/// Holder for sorted collections used in program - each data are
/// held in a vector-type container with specific sorting
/// sort key is a primary sorting key of the collection.
/// Collection is always sorted thus enabling binary search according 
/// to primary sorting
///\date 10-2006
///\author Jan 'Kovis' Struhar

#include "../data/functors.h"

#include <QList>
#include <algorithm>

class IdTag;

template<typename MemberType, typename MemberContainer, typename ContainerOrdering>
    class GenericIndexing
{
	typedef typename MemberContainer::iterator MemberContainerIt;
	typedef typename MemberContainer::const_iterator MemberContainerConstIt;
public:
	typedef ContainerOrdering tContainerOrdering;

    GenericIndexing() {};
    virtual ~GenericIndexing() {};

    static GenericIndexing& allSet()
	{
		return m_allSet;
	}

	static void releaseAllSet()
	{
		allSet().clear(true);
	}

	void addElements(const QList<MemberType>& clientList)
	{
		m_data += MemberContainer::fromList(clientList);
		sort();
	}

	bool addElement(MemberType pMember)
	{
		bool retVal = false;

		if (pMember)
		{
			if (find(pMember->getId()) == nullptr)
			{
				MemberContainerIt it = 
					std::lower_bound(m_data.begin(), m_data.end(), 
				pMember, ContainerOrdering());

				m_data.insert(it, pMember);
				retVal = true;
			}
		}
		return retVal;
	}

    virtual void deleteElement(MemberType const pToBeDeleted)
    {
        const int index = m_data.indexOf(pToBeDeleted);

        Q_ASSERT(index >= 0);
        if (index >= 0)
        {
            m_data.remove(index);
        }
    }

    virtual void setData(const MemberContainer& data)
	{
		m_data = data;
		sort();
	}

	/// value copy of container
	MemberContainer data() const
	{
		return m_data;
	}

	int size() const
	{
		return m_data.size();
	}

	virtual void sort()
	{
		std::sort(m_data.begin(), m_data.end(), ContainerOrdering());
	}

    virtual MemberType find(const IdTag& id) const
	{
		MemberType retVal = nullptr;

		MemberContainerConstIt it = std::find_if(m_data.begin(), m_data.end(), 
			IdEquality(id));
		if (it != m_data.end())
		{
			retVal = *it;
		}

		return retVal;
	}

	/// empties container with optional releasing of elements
	virtual void clear(const bool releaseElements)
	{
		if (releaseElements)
		{
			qDeleteAll(m_data);
		}
		m_data.clear();
	}

private:
    static GenericIndexing<MemberType, MemberContainer,
    ContainerOrdering> m_allSet;
	
	MemberContainer m_data;
};

template<typename MemberType, typename MemberContainer, 
        typename ContainerOrdering> GenericIndexing<MemberType, MemberContainer, ContainerOrdering>
        GenericIndexing<MemberType, MemberContainer, ContainerOrdering>::m_allSet;

#endif
