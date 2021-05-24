#ifndef ANDFILTER_H_INCLUDED_499FA0A9_34CC_4B70_B6F4_B086039B58C6
#define ANDFILTER_H_INCLUDED_499FA0A9_34CC_4B70_B6F4_B086039B58C6

///\class AndFilter
/// Compound filter that is true if every subfilter is true on a service.
/// Empty CAndFilter is true on empty subfilter (accepts anything)
///\date 11-2006
///\author Jan 'Kovis' Struhar
#include "IServiceFilter.h"
#include "HsTypedefs.h"

class AndFilter : public IServiceFilter
{
	typedef IServiceFilter super;
public:
    AndFilter();
    virtual ~AndFilter();

	/// takes ownership of filter
	void add(IServiceFilter *);

    virtual bool pass(const HolyService* const holyService) const override;

private:
	tServiceFilterVector m_subFilters;
};

#endif

