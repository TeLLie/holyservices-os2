/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ClientStorage.h"

#include "PersonStorage.h"
#include "Client.h"

#include <QDomDocument>

const QString ClientStorage::clientTagName("ServiceClient");
const QString ClientStorage::streetAttributeName("Street");
const QString ClientStorage::contactAttributeName("Contact");

ClientStorage::ClientStorage()
{
	setRootElementName(clientTagName);
}

ClientStorage::~ClientStorage()
{
}

QDomNode* ClientStorage::toXmlDom() const
{
	assert(m_saved);

	if (m_saved)
	{
		QDomDocument *pDoc = new QDomDocument;

		QDomElement rootElem = pDoc->createElement(rootElementName());
		pDoc->appendChild(rootElem);

		// data
		QDomElement dataElement = pDoc->createElement(dataElementName);

		dataElement.setAttribute(idAttributeName, m_saved->getId().toString());
		dataElement.setAttribute(PersonStorage::firstNameTagName, m_saved->firstName());
		dataElement.setAttribute(PersonStorage::surnameTagName, m_saved->surname());
                dataElement.setAttribute(streetAttributeName, m_saved->street());
                dataElement.setAttribute(contactAttributeName, m_saved->contact());

		rootElem.appendChild(dataElement);

		return pDoc; // will be released in CDomStorageQueue
	}
	return nullptr;
}

bool ClientStorage::fromXmlDom(const QDomNode *pNode)
{
	bool retVal = false;

	// previously loaded should be picked up or we counter a memory leak
	assert((m_loaded == nullptr) || m_wasPickedUp); 

	if (pNode)
	{
		QDomElement clientElement;

		// either node is document containing element as root one - 
		// this is just to ensure, that fromXmlDom understands toXmlDom()
		if (pNode->isDocument()) 
		{
			clientElement = pNode->toDocument().documentElement();
		}
		else if (pNode->isElement())
		{
			clientElement = pNode->toElement();
		}

		const bool nameFits = (! clientElement.isNull()) && 
			(clientElement.tagName() == rootElementName());
		assert(nameFits);
		
		if (nameFits)
		{
			m_loaded = new Client;

			QDomElement dataElement = clientElement.firstChildElement(dataElementName);
			if (! dataElement.isNull())
			{
				QString attrValue, attrName; // temp value

				attrName = idAttributeName;
				if (dataElement.hasAttribute(attrName))
				{
					attrValue = dataElement.attribute(attrName);
					const IdTag recordId = IdTag::createFromString(attrValue);

					m_loaded->setId(recordId);
				}
				
				attrName = PersonStorage::firstNameTagName;
				if (dataElement.hasAttribute(attrName))
				{
					attrValue = dataElement.attribute(attrName);
					m_loaded->setFirstName(attrValue);
				}
				
				attrName = PersonStorage::surnameTagName;
				if (dataElement.hasAttribute(attrName))
				{
					attrValue = dataElement.attribute(attrName);
					m_loaded->setSurname(attrValue);
				}

                                attrName = streetAttributeName;
                                if (dataElement.hasAttribute(attrName))
                                {
                                        attrValue = dataElement.attribute(attrName);
                                        m_loaded->setStreet(attrValue);
                                }

                                attrName = contactAttributeName;
                                if (dataElement.hasAttribute(attrName))
                                {
                                        attrValue = dataElement.attribute(attrName);
                                        m_loaded->setContact(attrValue);
                                }

				retVal = true;
			}

			m_wasPickedUp = false;
		}
	}

	if (retVal)
	{
		m_loaded->setClean(); // freshly read out of storage
	}
	else
	{
		delete m_loaded;
		m_loaded = nullptr;
	}

	return retVal;
}


