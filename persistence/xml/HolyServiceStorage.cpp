/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "HolyServiceStorage.h"

#include "HolyService.h"
#include "Client.h"
#include "Priest.h"
#include "Church.h"

#include <QDomDocument>

const QString HolyServiceStorage::holyServiceTagName("HolyService");
const QString HolyServiceStorage::intentionTagName("Intention");
const QString HolyServiceStorage::startTimeTagName("StartTime");
const QString HolyServiceStorage::endTimeTagName("EndTime");
const QString HolyServiceStorage::clientIdTagName("ClientReference");
const QString HolyServiceStorage::priestIdTagName("PriestReference");
const QString HolyServiceStorage::churchIdTagName("ChurchReference");
const QString HolyServiceStorage::timeConstraintTagName("TimeConstraint");
const QString HolyServiceStorage::receivedMoneyTagName("ReceivedMoney");

HolyServiceStorage::HolyServiceStorage(void)
{
    m_timeConstraintResolution[HolyService::FixedToDate] = "FixedToDate";
    m_timeConstraintResolution[HolyService::Movable] = "Movable";

	setRootElementName(holyServiceTagName);
}

HolyServiceStorage::~HolyServiceStorage(void)
{
}

QDomNode* HolyServiceStorage::toXmlDom() const
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
		dataElement.setAttribute(intentionTagName, m_saved->intention());
		dataElement.setAttribute(startTimeTagName, 
			m_saved->startTime().toString(Qt::ISODate));
		dataElement.setAttribute(endTimeTagName, 
			m_saved->endTime().toString(Qt::ISODate));
        dataElement.setAttribute(timeConstraintTagName,
            m_timeConstraintResolution.value(m_saved->timeConstraint()));
        dataElement.setAttribute(receivedMoneyTagName, QString::number(m_saved->receivedMoney(), 'f', 8));

		{
			IIdentifiable* serviceClient = m_saved->getClient();
			dataElement.setAttribute(clientIdTagName, 
				(serviceClient == nullptr) ? QString() : serviceClient->getId().toString());
		}

		{
			IIdentifiable* servicePriest = m_saved->getPriest();
			dataElement.setAttribute(priestIdTagName, 
				(servicePriest == nullptr) ? QString() : servicePriest->getId().toString());
		}

		{
			IIdentifiable* serviceChurch = m_saved->getChurch();
			dataElement.setAttribute(churchIdTagName, 
				(serviceChurch == nullptr) ? QString() : serviceChurch->getId().toString());
		}

		rootElem.appendChild(dataElement);

		// history
		QDomElement historyElement = pDoc->createElement(historyElementName);
		historyElement.setAttribute(recordCreatedAttributeName, 
			m_saved->created().toString(Qt::ISODate));
		historyElement.setAttribute(recordModifiedAttributeName, 
			m_saved->lastModified().toString(Qt::ISODate));
		historyElement.setAttribute(recordCreatorAttributeName, m_saved->creator());
		historyElement.setAttribute(recordModificatorAttributeName, m_saved->lastModificator());

		rootElem.appendChild(historyElement);

		return pDoc; // will be released in CDomStorageQueue
	}
	return nullptr;
}

bool HolyServiceStorage::fromXmlDom(const QDomNode *pNode)
{
	bool retVal = false;

	// previously loaded should be picked up or we counter a memory leak
	assert((m_loaded == nullptr) || m_wasPickedUp); 

	if (pNode)
	{
		QDomElement hsElement;

		// either node is document containing element as root one - 
		// this is just to ensure, that fromXmlDom understands toXmlDom()
		if (pNode->isDocument()) 
		{
			hsElement = pNode->toDocument().documentElement();
		}
		else if (pNode->isElement())
		{
			hsElement = pNode->toElement();
		}

		const bool nameFits = (! hsElement.isNull()) && 
			(hsElement.tagName() == rootElementName());
		assert(nameFits);
		
		if (nameFits)
		{
			m_loaded = new HolyService;

			QDomElement dataElement = hsElement.firstChildElement(dataElementName);
			if (! dataElement.isNull())
			{
				QString attrValue; // temp value

				if (dataElement.hasAttribute(idAttributeName))
				{
					attrValue = dataElement.attribute(idAttributeName);
					const IdTag recordId = IdTag::createFromString(attrValue);

					m_loaded->setId(recordId);
				}
				if (dataElement.hasAttribute(intentionTagName))
				{
					m_loaded->setIntention(dataElement.attribute(intentionTagName));
				}
				if (dataElement.hasAttribute(startTimeTagName))
				{
					attrValue = dataElement.attribute(startTimeTagName);
					m_loaded->setStartTime(QDateTime::fromString(attrValue, Qt::ISODate));
				}
				if (dataElement.hasAttribute(endTimeTagName))
				{
					attrValue = dataElement.attribute(endTimeTagName);
					m_loaded->setEndTime(QDateTime::fromString(attrValue, Qt::ISODate));
				}
				if (dataElement.hasAttribute(clientIdTagName))
				{
					attrValue = dataElement.attribute(clientIdTagName);
					if (! attrValue.isEmpty())
					{
						m_loaded->setClientId(IdTag::createFromString(attrValue));
					}
				}
				if (dataElement.hasAttribute(priestIdTagName))
				{
					attrValue = dataElement.attribute(priestIdTagName);
					if (! attrValue.isEmpty())
					{
						m_loaded->setPriestId(IdTag::createFromString(attrValue));
					}
				}
				if (dataElement.hasAttribute(churchIdTagName))
				{
					attrValue = dataElement.attribute(churchIdTagName);
					if (! attrValue.isEmpty())
					{
						m_loaded->setChurchId(IdTag::createFromString(attrValue));
					}
				}
                if (dataElement.hasAttribute(timeConstraintTagName))
                {
                    attrValue = dataElement.attribute(timeConstraintTagName);
                    m_loaded->setTimeConstraint(m_timeConstraintResolution.
                        key(attrValue, HolyService::FixedToDate));
                }
                if (dataElement.hasAttribute(receivedMoneyTagName))
                {
                    attrValue = dataElement.attribute(receivedMoneyTagName);
                    m_loaded->setReceivedMoney(attrValue.toDouble());
                }

				retVal = true;
			}

			QDomElement historyElement = hsElement.firstChildElement(historyElementName);

			assert(! (dataElement.isNull() || historyElement.isNull()));

			if (! historyElement.isNull())
			{
				QString attrValue; // temp value

				if (historyElement.hasAttribute(recordCreatedAttributeName))
				{
					attrValue = historyElement.attribute(recordCreatedAttributeName);
					m_loaded->setCreated(QDateTime::fromString(attrValue, Qt::ISODate));
				}
				if (historyElement.hasAttribute(recordCreatorAttributeName))
				{
					attrValue = historyElement.attribute(recordCreatorAttributeName);
					m_loaded->setCreator(attrValue);
				}
				if (historyElement.hasAttribute(recordModifiedAttributeName))
				{
					attrValue = historyElement.attribute(recordModifiedAttributeName);
					m_loaded->setLastModified(QDateTime::fromString(attrValue, Qt::ISODate));
				}
				if (historyElement.hasAttribute(recordModificatorAttributeName))
				{
					attrValue = historyElement.attribute(recordModificatorAttributeName);
					m_loaded->setLastModificator(attrValue);
				}
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
