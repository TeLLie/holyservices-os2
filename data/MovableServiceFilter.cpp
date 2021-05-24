#include "MovableServiceFilter.h"

#include "HolyService.h"

MovableServiceFilter::MovableServiceFilter()
{
}

MovableServiceFilter::~MovableServiceFilter()
{
}

bool MovableServiceFilter::pass(const HolyService *const holyService) const
{
    return holyService && (holyService->timeConstraint() == HolyService::Movable);
}

