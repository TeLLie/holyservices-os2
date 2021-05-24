#ifndef PERSON_H_INCLUDED_90256C56_7AAB_4185_812A_86A79FFF7701
#define PERSON_H_INCLUDED_90256C56_7AAB_4185_812A_86A79FFF7701

///\class Person
///\brief generic identifiable, dirty tracking person, designed for derivation
///\author Jan 'Kovis' Struhar
///\date 9-2006

#include "BasicModificationAware.h"
#include "Identifiable.h"

class Person : public BasicModificationAware, public IIdentifiable
{
public:
    enum TextFormat { NameSurname, SurnameNamePlain };

	virtual ~Person();

    virtual const IdTag& getId() const;
    virtual void setId(const IdTag&);

	virtual const QString& firstName() const;
	virtual void setFirstName(const QString& name);

	virtual const QString& surname() const;
	virtual void setSurname(const QString& name);

	virtual QString toString(TextFormat fmt = NameSurname) const;

protected:
    Person();

private:
    QString m_firstName;
	QString m_surname;
    Identifiable m_id;
};

#endif
