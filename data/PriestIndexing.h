#ifndef PRIESTINDEXING_H_INCLUDED_766BDA6B_7BD0_4192_AFEC_0B5AAC4F9225
#define PRIESTINDEXING_H_INCLUDED_766BDA6B_7BD0_4192_AFEC_0B5AAC4F9225

///\class PriestIndexing
///\brief Priest container
///\date 10-2006
///\author Jan 'Kovis' Struhar

#include "Priest.h"

#include "../common/GenericIndexing.h"
#include "HsTypedefs.h"
#include "functors.h"

class Priest;

class PriestIndexing : public GenericIndexing<Priest *, tPriestVector, PersonSurnameAscendingSorting>
{
public:
        typedef GenericIndexing<Priest *, tPriestVector, PersonSurnameAscendingSorting> super;

    PriestIndexing();
    virtual ~PriestIndexing();

	static bool hasDuplicities(const super& data);
	
	/// decide based on usage in holyservices - not used clients are marked to be removed
	/// if for some record would all data erased (brand new record is entered twice),
	/// with option leaveOneSurvivor on true - one record is skipped
	static tPriestVector findDuplicities(super& data, bool leaveOneSurvivor);
};

#endif
