/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "BasicXmlTransformer.h"

#include <QTextStream>
#include <QDomDocument>

BasicXmlTransformer::BasicXmlTransformer(void)
{
}

BasicXmlTransformer::~BasicXmlTransformer(void)
{
}

QString BasicXmlTransformer::toXmlString() const
{
	QString retVal;
	QDomNode *pSrcDom = toXmlDom();

	if (pSrcDom)
	{
		QTextStream ts(& retVal);

		ts << *pSrcDom;
	}
	return retVal;
}

bool BasicXmlTransformer::fromXmlString(const QString& xmlMessage)
{
	// convert to DOM node
	QDomDocument doc;

	if (doc.setContent(xmlMessage))
	{
		return fromXmlDom(&doc);
	}
	return false;
}
