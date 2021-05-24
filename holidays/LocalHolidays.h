#ifndef LOCALHOLIDAYS_H
#define LOCALHOLIDAYS_H

///\class LocalHolidays
///\brief Read out locale specific file from the holidays folder
///\date 11-2010

#include "IHolidays.h"

class LocalHolidays : public IHolidays
{
    typedef IHolidays super;
public:
    LocalHolidays();
    virtual ~LocalHolidays();

    virtual bool isHoliday(const QDate& date, QStringList *description) const;

    void loadFile(const QString& holidayFileName);
    virtual void refresh();

private:
    struct LocalHolidaysImpl;
    LocalHolidaysImpl* pImpl;
};

#endif // LOCALHOLIDAYS_H
