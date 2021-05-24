#ifndef PERSONSTORAGE_H_INCLUDED_B0196176_2D64_4FE3_B9AC_770AEF37EC2A
#define PERSONSTORAGE_H_INCLUDED_B0196176_2D64_4FE3_B9AC_770AEF37EC2A

#include <QString>

///\class PersonStorage
/// Class does not store anything, only holds tag names for person attributes
/// the sole person is not stored within the app.
///\date 9-2006
///\author Jan 'Kovis' Struhar

class PersonStorage
{
public:
	static const QString firstNameTagName;
	static const QString surnameTagName;

private:
    PersonStorage();
    virtual ~PersonStorage();
};

#endif
