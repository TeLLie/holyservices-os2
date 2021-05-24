#include "IExporter.h"

IExporter::IExporter() : m_holidayProvider(0) {}

IExporter::~IExporter() {}

void IExporter::writeBlankLine(const QDate& /*dt*/, int /*options*/) {}

void IExporter::setHolidayProvider(const IHolidays* newProvider)
{
    m_holidayProvider = newProvider;
}

const IHolidays *IExporter::holidayProvider() const
{
    return m_holidayProvider;
}


