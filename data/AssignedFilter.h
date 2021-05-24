#ifndef ASSIGNEDFILTER_H_INCLUDED_FB7D9C2E_EE0B_4C0C_B8E2_C177ED21F42F
#define ASSIGNEDFILTER_H_INCLUDED_FB7D9C2E_EE0B_4C0C_B8E2_C177ED21F42F

#include "IServiceFilter.h"

///\class AssignedFilter
/// Filter accepts holy service that has been assigned to a client
///\author Jan 'Kovis' Struhar
///\date 9/2006

class AssignedFilter : public IServiceFilter
{
public:
    virtual bool pass(const HolyService* const holyService) const override;
    static bool isServiceAssigned(const HolyService& hs);
};

#endif
