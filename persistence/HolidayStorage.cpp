#include "HolidayStorage.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QStringList>

HolidayStorage::HolidayStorage() {}
HolidayStorage::~HolidayStorage() {}

bool HolidayStorage::storeToFile(const QString &fileName, const QList<HolidayRecord> &data)
{
    QFile f(fileName);
    const QFileInfo fi(fileName);
    if (! fi.dir().exists())
    {
        fi.dir().mkpath(fi.absolutePath());
    }

    if (f.open(QIODevice::WriteOnly))
    {
        QTextStream ts(&f);
        ts.setCodec("UTF-8");

        foreach (const HolidayRecord& r, data)
        {
            ts << r.toString() << "\n";
        }

        f.close();
        return true;
    }
    return false;
}

bool HolidayStorage::storeToFile(const QString& fileName, const QStringList& data)
{
    QList<HolidayRecord> converted;

    foreach (const QString str, data)
    {
        HolidayRecord r;

        if (r.fromString(str))
        {
            converted.append(r);
        }
    }
    return storeToFile(fileName, converted);
}

bool HolidayStorage::loadFromFile(const QString &fileName, QList<HolidayRecord> &data)
{
    QFile f(fileName);

    data.clear();

    if (f.open(QIODevice::ReadOnly))
    {
        QTextStream ts(&f);
        ts.setCodec("UTF-8");

        HolidayRecord holiday;

        while (! ts.atEnd())
        {
            if (holiday.fromString(ts.readLine()))
            {
                data.append(holiday);
            }
        }
        f.close();

        qSort(data);

        return true;
    }
    return false;
}
