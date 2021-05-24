#ifndef CHURCHFILTER_H_INCLUDED_5F1C3F82_F29E_4152_AD11_9D8113DD752F
#define CHURCHFILTER_H_INCLUDED_5F1C3F82_F29E_4152_AD11_9D8113DD752F

///\class ChurchFilter
///\brief Filter lets pass the holy service that takes place in the church the filter
/// was initialized with
///\date 11-2006
///\author Jan 'Kovis' Struhar
#include "IServiceFilter.h"
#include "../common/IdTag.h"

class ChurchFilter : public IServiceFilter
{
	typedef IServiceFilter super;
public:
    ChurchFilter(const IdTag& m_id);
    virtual ~ChurchFilter();

    virtual bool pass(const HolyService* const holyService) const override;
private:
	IdTag m_id;
};

#endif
