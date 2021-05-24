#ifndef HOLIDAYCOMPOSITION_H
#define HOLIDAYCOMPOSITION_H

///\class HolidayComposition
///\brief Combination of common christian holidays and locale ones behind one interface
/// Potentially can be the members disabled if someone does not want to see
/// locale holidays or other provider
///\date 11-2010

#include "IHolidays.h"

class CommonChristianHolidays;
class LocalHolidays;

class HolidayComposition : public IHolidays
{
public:
    HolidayComposition(const QString& sharedAppFolder, const QString& userWritableDataFolder);
    virtual ~HolidayComposition();

    virtual bool isHoliday(const QDate& date, QStringList *description) const;
    virtual void refresh();

    /// locale specific holiday file located in shared data read-only folder in installation time
    static QString localHolidaysFileName(const QString& sharedAppFolder);

    /// locale specific per user in a writable location that can be written from application
    static QString userSpecificHolidaysFileName(const QString& userWritableDataFolder);

private:
    CommonChristianHolidays* m_christianHolidays = nullptr;
    LocalHolidays* m_localReadOnlyHolidays = nullptr;
    LocalHolidays* m_localWritableHolidays = nullptr;
};

#endif // HOLIDAYCOMPOSITION_H
