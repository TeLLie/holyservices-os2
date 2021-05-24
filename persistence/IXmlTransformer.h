#pragma once

///\class IXmlTransformer
/// Interface for classes that can transform self to/from a DOM node.
/// They can do it for own persistence purpose.
///\author Jan 'Kovis' Struhar
///\date 8-2006

#include <QString>

class QDomNode;

class IXmlTransformer
{
	virtual QString toXmlString() const = 0;
	virtual bool fromXmlString(const QString& xmlMessage) = 0;

	virtual QDomNode* toXmlDom() const = 0;
	virtual bool fromXmlDom(const QDomNode *) = 0;

protected:
	IXmlTransformer() {};
	virtual ~IXmlTransformer() {};
};

