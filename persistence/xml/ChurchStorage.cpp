/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ChurchStorage.h"

#include "Church.h"
#include "PriestIndexing.h"

#include <QDomDocument>

const QString ChurchStorage::churchTagName("Church");
const QString ChurchStorage::nameTagName("Name");
const QString ChurchStorage::abbreviationTagName("Abbreviation");
const QString ChurchStorage::placeTagName("Place");
const QString ChurchStorage::colorTagName("Color");
const QString ChurchStorage::usualServiceTagName("UsualService");
const QString ChurchStorage::activeTagName("Active");
const QString ChurchStorage::priestTagName("Priest");
const QString ChurchStorage::weekDayTagName("Weekday");
const QString ChurchStorage::startTagName("StartTime");
const QString ChurchStorage::durationTagName("Duration");
const QString ChurchStorage::eastingTagName("Easting");
const QString ChurchStorage::northingTagName("Northing");

ChurchStorage::ChurchStorage()
{
	setRootElementName(churchTagName);
}

ChurchStorage::~ChurchStorage()
{
}

QDomNode* ChurchStorage::toXmlDom() const
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
        dataElement.setAttribute(nameTagName, m_saved->name());
        dataElement.setAttribute(abbreviationTagName, m_saved->abbreviation());
        dataElement.setAttribute(placeTagName, m_saved->place());
        dataElement.setAttribute(colorTagName, m_saved->color().name());
        dataElement.setAttribute(eastingTagName, QString::number(m_saved->easting(), 'g', 12));
        dataElement.setAttribute(northingTagName, QString::number(m_saved->northing(), 'g', 12));

		foreach(const Church::ServiceTemplate& service, m_saved->usualServices())
		{
			QDomElement usualServiceElement = pDoc->createElement(usualServiceTagName);

			usualServiceElement.setAttribute(activeTagName, service.m_active);
                        usualServiceElement.setAttribute(weekDayTagName, service.m_serviceTerm);
			usualServiceElement.setAttribute(startTagName, service.m_start.toString(Qt::ISODate));
			usualServiceElement.setAttribute(durationTagName, service.m_durationInMinutes);

                        {
                        IIdentifiable* servicePriest = PriestIndexing::allSet().find(service.m_priest);
                        usualServiceElement.setAttribute(priestTagName, (servicePriest != nullptr) ? service.m_priest.toString() : QString());
                        }

			dataElement.appendChild(usualServiceElement);
		}

		rootElem.appendChild(dataElement);

		return pDoc; // will be released in CDomStorageQueue
	}
	return nullptr;
}

bool ChurchStorage::fromXmlDom(const QDomNode *pNode)
{
	bool retVal = false;

	// previously loaded should be picked up or we counter a memory leak
	assert((m_loaded == nullptr) || m_wasPickedUp); 

	if (pNode)
	{
		QDomElement churchElement;

		// either node is document containing element as root one - 
		// this is just to ensure, that fromXmlDom understands toXmlDom()
		if (pNode->isDocument()) 
		{
			churchElement = pNode->toDocument().documentElement();
		}
		else if (pNode->isElement())
		{
			churchElement = pNode->toElement();
		}

		const bool nameFits = (! churchElement.isNull()) && 
			(churchElement.tagName() == rootElementName());
		assert(nameFits);
		
		if (nameFits)
		{
			m_loaded = new Church;

			QDomElement dataElement = churchElement.firstChildElement(dataElementName);
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
				
				attrName = nameTagName;
				if (dataElement.hasAttribute(attrName))
				{
					attrValue = dataElement.attribute(attrName);
					m_loaded->setName(attrValue);
				}
				
                attrName = abbreviationTagName;
                if (dataElement.hasAttribute(attrName))
                {
                    attrValue = dataElement.attribute(attrName);
                    m_loaded->setAbbreviation(attrValue);
                }

				attrName = placeTagName;
				if (dataElement.hasAttribute(attrName))
				{
					attrValue = dataElement.attribute(attrName);
					m_loaded->setPlace(attrValue);
				}

				attrName = colorTagName;
				if (dataElement.hasAttribute(attrName))
				{
					attrValue = dataElement.attribute(attrName);
					
					QColor churchColor;
					churchColor.setNamedColor(attrValue);
					m_loaded->setColor(churchColor);
				}

                attrName = eastingTagName;
                if (dataElement.hasAttribute(attrName))
                {
                    attrValue = dataElement.attribute(attrName);
                    m_loaded->setEasting(attrValue.toDouble());
                }

                attrName = northingTagName;
                if (dataElement.hasAttribute(attrName))
                {
                    attrValue = dataElement.attribute(attrName);
                    m_loaded->setNorthing(attrValue.toDouble());
                }

                Church::tServiceTemplateList usualServices;
				for (QDomNode serviceNode = dataElement.firstChildElement(usualServiceTagName);
					(! serviceNode.isNull()); 
					serviceNode = serviceNode.nextSiblingElement(usualServiceTagName))
				{
					const QDomElement serviceElement = serviceNode.toElement();
					Church::tServiceTemplateList::value_type aService;

					attrName = activeTagName;
					if (serviceElement.hasAttribute(attrName))
					{
						attrValue = serviceElement.attribute(attrName);
						aService.m_active = (attrValue == "1");
					}
					attrName = priestTagName;
					if (serviceElement.hasAttribute(attrName))
					{
						attrValue = serviceElement.attribute(attrName);
						if (! attrValue.isEmpty())
						{
							aService.m_priest.setIdFromString(attrValue);
						}
					}
					attrName = weekDayTagName;
					if (serviceElement.hasAttribute(attrName))
					{
						attrValue = serviceElement.attribute(attrName);
                                                aService.m_serviceTerm =
                                                        static_cast<Church::ServiceTemplate::ServiceTerm>(attrValue.toInt());
					}
					attrName = startTagName;
					if (serviceElement.hasAttribute(attrName))
					{
						attrValue = serviceElement.attribute(attrName);
						aService.m_start = QTime::fromString(attrValue, Qt::ISODate);
					}
					attrName = durationTagName;
					if (serviceElement.hasAttribute(attrName))
					{
						attrValue = serviceElement.attribute(attrName);
						aService.m_durationInMinutes = attrValue.toInt();
					}
					// reading of service template completed

					usualServices << aService;
				}
				if (! usualServices.isEmpty())
				{
					m_loaded->setUsualServices(usualServices);
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

