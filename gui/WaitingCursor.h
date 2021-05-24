#pragma once

///\class WaitingCursor
///\author Jan 'Kovis' Struhar
///\date 7/2005
///\brief Turns on hour-glass cursor and restores it on its destruction - thus watching
///       proper mouse pointer restoration
class WaitingCursor
{
public:
    WaitingCursor(void);
    virtual ~WaitingCursor(void);
};
