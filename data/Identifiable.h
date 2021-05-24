#pragma once
#include "IIdentifiable.h"
#include "../common/IdTag.h"

///\class Identifiable
///\brief Basic implementation of IIdentifiable interface
///\author Jan 'Kovis' Struhar
///\date 8-2006
class Identifiable : public IIdentifiable
{
public:
    Identifiable(void);
    virtual ~Identifiable(void);

	virtual const IdTag& getId() const;
	virtual void setId(const IdTag&);

private:
	IdTag m_id;
};
