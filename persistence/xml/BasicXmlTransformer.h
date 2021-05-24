#pragma once
#include "IXmlTransformer.h"

///\class BasicXmlTransformer
/// Common implementation of IXmlTransformer class. Class is designed 
/// to be inherited into specialized implementations, cannot be used directly
///\author Jan 'Kovis' Struhar
///\date 9-2006

class BasicXmlTransformer : public IXmlTransformer
{
public:
    virtual ~BasicXmlTransformer(void);

	virtual QString toXmlString() const;
	virtual bool fromXmlString(const QString& xmlMessage);

	/// these two methods have to be overloaded by ancestors
	virtual QDomNode* toXmlDom() const = 0;
	virtual bool fromXmlDom(const QDomNode *) = 0;

protected:
    BasicXmlTransformer(void);
};
