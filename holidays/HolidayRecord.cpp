#include "HolidayRecord.h"

#include <QRegExp>

HolidayRecord::HolidayRecord()
{
}

const QDate& HolidayRecord::date() const
{
    return m_date;
}

void HolidayRecord::setDate(const QDate& val)
{
    m_date = val;
}

const QString& HolidayRecord::description() const
{
    return m_description;
}

void HolidayRecord::setDescription(const QString& val)
{
    m_description = val;
}

QString HolidayRecord::toString() const
{
    return QString("%1 %2").arg(m_date.toString(Qt::ISODate), m_description);
}

bool HolidayRecord::fromString(const QString& line)
{
    QRegExp re("(\\d{4}-\\d{2}-\\d{2})\\s+(.*)\n?"); // ISO date and description separated by whitespace

    if (re.indexIn(line) >= 0)
    {
        const QString dateText = re.cap(1);
        m_date = QDate::fromString(dateText, Qt::ISODate);
        m_description = re.cap(2);

        return m_date.isValid();
    }
    return false;
}

bool HolidayRecord::operator ==(const HolidayRecord &other) const
{
    return (this->m_date == other.m_date) && (this->m_description == other.m_description);
}
