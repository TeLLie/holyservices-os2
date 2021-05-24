#ifndef IHOLIDAYS_H_INCLUDED_7CE65749_865E_457C_BF71_1566A8B3FD76
#define IHOLIDAYS_H_INCLUDED_7CE65749_865E_457C_BF71_1566A8B3FD76

///\class IHolidays
///\brief Tells about state holidays and commonly free days (Saturdays, Sundays)
///\date 9-2006
///author Jan 'Kovis' Struhar

#include <QDate>
#include <QString>

class IHolidays
{
	public:
	virtual ~IHolidays() {};

    /// most important method of interface saying if certain day is a holiday and what is its description
    virtual bool isHoliday(const QDate& date, QStringList *description) const = 0;

    /// for some implementations makes sense to "reload" the data
    virtual void refresh() {};
};

#endif
