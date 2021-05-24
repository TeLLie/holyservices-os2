#ifndef CLIENTINDEXING_H_INCLUDED_CD4F03CC_1C9E_4D41_85A3_EA2882EE0443
#define CLIENTINDEXING_H_INCLUDED_CD4F03CC_1C9E_4D41_85A3_EA2882EE0443

#include "Client.h"

#include "../common/GenericIndexing.h"
#include "HsTypedefs.h"
#include "functors.h"

///\class ClientIndexing
/// sorting is surname ascending
/// Class implements means to detect and identify duplicities 
/// in client name, surname and street (value duplicities)
///\date 9-2006
class ClientIndexing :
    public GenericIndexing<Client *, tClientVector, ClientSurnameAscendingSorting>
{
public:
    typedef GenericIndexing<Client *, tClientVector, ClientSurnameAscendingSorting> super;

    ClientIndexing();
    virtual ~ClientIndexing();

	static bool hasDuplicities(const super& data);
	
	/// decide based on usage in holyservices - not used clients are marked to be removed
	/// if for some record would all data erased (brand new record is entered twice),
	/// with option leaveOneSurvivor on true - one record is skipped
	static tClientVector findDuplicities(super& data, bool leaveOneSurvivor);
};

#endif
