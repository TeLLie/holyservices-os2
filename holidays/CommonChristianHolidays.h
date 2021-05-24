#ifndef COMMONCHRISTIANHOLIDAYS_H
#define COMMONCHRISTIANHOLIDAYS_H

///\class CommonChristianHolidays
///\brief Program hardcoded general festive days like eastern, christmas
///\date 11-2010

#include "IHolidays.h"
#include <QMap>

class CommonChristianHolidays : public IHolidays
{
public:
    CommonChristianHolidays();
    virtual ~CommonChristianHolidays();

    virtual bool isHoliday(const QDate& date, QStringList *description) const;

private:
    QMap<QDate, QString> m_easterRelatedHolidays;

    /// initialize easter related holidays (for years 1990-2050)
    void initEasterRelatedHolidays();

    /// algorithm valid for years 1583-4099 taken from
    /// http://www.assa.org.au/edm.html#Computer
    /// This algorithm is an arithmetic interpretation of the 3 step
    /// Easter Dating Method developed by Ron Mallen 1985, as a vast
    /// improvement on the method described in the Common Prayer Book
	/// rwmallen@chariot.net.au
    static bool gregorianEasterSunday(const int year, int& day, int& month);
};

#endif // COMMONCHRISTIANHOLIDAYS_H
