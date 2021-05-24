#ifndef CLIENTSTORAGE_H_INCLUDED_738F0625_DB9E_4559_A89B_5424A71C6026
#define CLIENTSTORAGE_H_INCLUDED_738F0625_DB9E_4559_A89B_5424A71C6026

///\class ClientStorage
/// The class is in charge for transforming of the holy service client data
/// to/from a DOM node (XML fragment)
///\author Jan 'Kovis' Struhar
///\date 9-2006
#include "BasicXmlTransformer.h"
#include "BasicClassStorage.h"

class Client;

class ClientStorage : public BasicClassStorage<Client>, public BasicXmlTransformer
{
public:
	static const QString clientTagName;

    ClientStorage();
    virtual ~ClientStorage();

	virtual QDomNode* toXmlDom() const;
	virtual bool fromXmlDom(const QDomNode *);

private:
    static const QString streetAttributeName;
    static const QString contactAttributeName;
};

#endif
