#pragma once

///\class HolyServiceStorage
/// The class is in charge for transforming of the holy service data
/// to/from a DOM node (XML fragment)
///\author Jan 'Kovis' Struhar
///\date 9-2006

#include "BasicClassStorage.h"
#include "BasicXmlTransformer.h"
#include "HolyService.h"
#include <QHash>

class HolyServiceStorage :	public BasicClassStorage<HolyService>, public BasicXmlTransformer
{
public:
	HolyServiceStorage(void);
	virtual ~HolyServiceStorage(void);

	/// class identifier in persistent storage
	static const QString holyServiceTagName;

	virtual QDomNode* toXmlDom() const;
	virtual bool fromXmlDom(const QDomNode *);

private:
	static const QString intentionTagName;
	static const QString startTimeTagName;
	static const QString endTimeTagName;
	static const QString clientIdTagName;
	static const QString priestIdTagName;
    static const QString churchIdTagName;
    static const QString timeConstraintTagName;
    static const QString receivedMoneyTagName;

    QHash<HolyService::TimeConstraint, QString> m_timeConstraintResolution;
};
