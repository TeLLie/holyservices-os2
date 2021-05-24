#ifndef CHURCHSTORAGE_H_INCLUDED_FBC0317D_9565_42A8_9B6E_5A8256C09BD3
#define CHURCHSTORAGE_H_INCLUDED_FBC0317D_9565_42A8_9B6E_5A8256C09BD3

///\class ChurchStorage
///\brief Church storage class implementation
///\date 10-2006
///\author Jan 'Kovis' Struhar

#include "BasicClassStorage.h"
#include "BasicXmlTransformer.h"

class Church;

class ChurchStorage : public BasicClassStorage<Church>, public BasicXmlTransformer
{
public:
	static const QString churchTagName;
    static const QString nameTagName;
    static const QString abbreviationTagName;
    static const QString placeTagName;
	static const QString colorTagName;
	static const QString usualServiceTagName;
	static const QString activeTagName;
	static const QString priestTagName;
	static const QString weekDayTagName;
	static const QString startTagName;
    static const QString durationTagName;
    static const QString eastingTagName;
    static const QString northingTagName;

    ChurchStorage();
    virtual ~ChurchStorage();

	virtual QDomNode* toXmlDom() const;
	virtual bool fromXmlDom(const QDomNode *);
};

#endif
