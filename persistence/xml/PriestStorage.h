#pragma once

///\class PriestStorage
/// The class is in charge for transforming of the priest information
/// to/from a DOM node (XML fragment)
///\author Jan 'Kovis' Struhar
///\date 9-2006

#include "BasicClassStorage.h"
#include "BasicXmlTransformer.h"

class Priest;

class PriestStorage : public BasicClassStorage<Priest>, public BasicXmlTransformer
{
public:
	static const QString priestTagName;

    PriestStorage(void);
    virtual ~PriestStorage(void);

	virtual QDomNode* toXmlDom() const;
	virtual bool fromXmlDom(const QDomNode *);
};

