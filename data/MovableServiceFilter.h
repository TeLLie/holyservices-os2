#ifndef MOVABLESERVICEFILTER_H_INCLUDED_7333F6C1_43BC_4D87_A752_2549CBB03444
#define MOVABLESERVICEFILTER_H_INCLUDED_7333F6C1_43BC_4D87_A752_2549CBB03444

///\class MovableServiceFilter
/// Pick out the services with time constraint Movable
///\date 10-2011
///\author Jan 'Kovis' Struhar jstruhar@amberg.cz
#include "IServiceFilter.h"

class MovableServiceFilter : public IServiceFilter
{
	typedef IServiceFilter super;
public:
	MovableServiceFilter();
	virtual ~MovableServiceFilter();

	virtual bool pass(const HolyService* const holyService) const;
	
private:

};

#endif

