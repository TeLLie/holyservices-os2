#ifndef CLIENTFILTER_H_INCLUDED_A4DFFCA3_A634_4EB9_824B_28FF1C33F383
#define CLIENTFILTER_H_INCLUDED_A4DFFCA3_A634_4EB9_824B_28FF1C33F383

///\class ClientFilter
/// Service passes, if the client id is the same as on the filter
///\date 10-2006
///\author Jan 'Kovis' Struhar
#include "IServiceFilter.h"

class IdTag;

class ClientFilter : public IServiceFilter
{
	typedef IServiceFilter super;
public:
    ClientFilter(const IdTag& clientId);
    virtual ~ClientFilter();

	virtual bool pass(const HolyService* const holyService) const;

private:
	const IdTag& m_id;
    ClientFilter& operator =(const ClientFilter&);
};

#endif
