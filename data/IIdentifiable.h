#pragma once

class IdTag;

///\class IIdentifiable
/// Interface defining an object that can identify itself by CIdTag
///\author Jan 'Kovis' Struhar
///\date 8-2006
//
class IIdentifiable
{
public:
	virtual const IdTag& getId() const = 0;
	virtual void setId(const IdTag&) = 0;

	virtual ~IIdentifiable() {}
};


