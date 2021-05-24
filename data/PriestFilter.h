#ifndef PRIESTFILTER_H_INCLUDED_C863F7F9_EFD3_434C_AAE9_3CB359AA3F25
#define PRIESTFILTER_H_INCLUDED_C863F7F9_EFD3_434C_AAE9_3CB359AA3F25

///\class PriestFilter
///\brief filter leaves just holy services served by given priest
///\date 11-2006
///\author Jan 'Kovis' Struhar
#include "IServiceFilter.h"
#include "../common/IdTag.h"

class PriestFilter : public IServiceFilter
{
	typedef IServiceFilter super;
public:
    PriestFilter(const IdTag& soughtPriestId);
    virtual ~PriestFilter();

	virtual bool pass(const HolyService* const holyService) const;

private:
	IdTag m_id;
};

#endif
