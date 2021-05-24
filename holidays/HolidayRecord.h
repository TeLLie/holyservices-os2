#ifndef HOLIDAYRECORD_H
#define HOLIDAYRECORD_H

#include <QDate>

class HolidayRecord
{
public:
    HolidayRecord();

    const QDate& date() const;
    void setDate(const QDate&);

    const QString& description() const;
    void setDescription(const QString&);

    /// String format tht can be saved to HolidayStorage
    QString toString() const;

    bool fromString(const QString& line);

    bool operator<(const HolidayRecord& other) const
    {
        return this->m_date < other.m_date;
    }

    bool operator ==(const HolidayRecord& other) const;
private:
    QDate m_date;
    QString m_description;
};

#endif // HOLIDAYRECORD_H
