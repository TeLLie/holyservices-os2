#ifndef CLIENT_H_INCLUDED_B2F1F165_B2B1_46B9_8D56_E382E5C2481F
#define CLIENT_H_INCLUDED_B2F1F165_B2B1_46B9_8D56_E382E5C2481F

#include "Person.h"

///\class Client
///\brief Holy service client (a parish member)
/// Client is a person that lives somewhere - it is one of basic classes
/// in the application, represents a man who has a holy service intention
/// aimed for certain date.
///\author Jan 'Kovis' Struhar
///\date 9-2006

class Client : public Person
{
public:
    Client();
    virtual ~Client();

    const QString& street() const;
    void setStreet(const QString&);

    const QString& contact() const;
    void setContact(const QString& val);

    enum TextFormat { SurnameStreet, NameSurnameStreet, NameStreet, SurnameName,
                      SurnameNameStreet,
                      SurnameNameStreetPlain /*compound formatting for sorting and comparisons*/
                    };
    virtual QString toString(const TextFormat = NameSurnameStreet) const;

private:
    QString m_street;
    QString m_contact; ///< form of quick contact to the client, usually cell phone number
};

#endif
