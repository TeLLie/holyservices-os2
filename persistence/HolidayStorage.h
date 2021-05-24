#ifndef HOLIDAYSTORAGE_H
#define HOLIDAYSTORAGE_H

///\class HolidayStorage
///\brief local holidays store/load
/// File format is a UTF-8 encoded text file
/// with lines <ISO-date><whitespace><description><CR>
/// like 2010-08-15 Virgin Mary<CR>

#include "HolidayRecord.h"

class HolidayStorage
{
public:
    /// loads data from a file sorted by date ascendingly
    static bool loadFromFile(const QString& fileName, QList<HolidayRecord>& data);
    static bool storeToFile(const QString& fileName, const QStringList& data);
    static bool storeToFile(const QString& fileName, const QList<HolidayRecord>& data);

private:
    HolidayStorage();
    virtual ~HolidayStorage();
};

#endif // HOLIDAYSTORAGE_H
