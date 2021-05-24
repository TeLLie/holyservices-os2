#ifndef CHURCHINDEXING_H_INCLUDED_75758E12_5C95_45BE_B820_4D5A54BFC849
#define CHURCHINDEXING_H_INCLUDED_75758E12_5C95_45BE_B820_4D5A54BFC849

///\class ChurchIndexing
/// Churches sorted by place and name, some methods for 
/// value duplicities detection and marking
///\date 10-2006
///\author Jan 'Kovis' Struhar

#include "Church.h"

#include "../common/GenericIndexing.h"
#include "HsTypedefs.h"
#include "functors.h"

class ChurchIndexing : public GenericIndexing<Church *, tChurchVector, PlaceNameSorting>
{
public:
    typedef GenericIndexing<Church *, tChurchVector, PlaceNameSorting> super;

	static bool hasDuplicities(const super& data);
	
	/// decide based on usage in holyservices - not used churches are marked to be removed
	/// if for some record would all data erased - (brand new record is entered twice),
	/// with option leaveOneSurvivor on true - one record is skipped
	static tChurchVector findDuplicities(super& data, bool leaveOneSurvivor);

	ChurchIndexing();
	virtual ~ChurchIndexing();
};

#endif
