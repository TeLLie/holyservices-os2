#ifndef DATERANGEFILTER_H
#define DATERANGEFILTER_H

/*!
  class DateRangeFilter
  Filters services with start date between from and to inclusive limits
  author Jan 'Kovis' Struhar
*/

#include "IServiceFilter.h"
#include <QDate>

class DateRangeFilter : public IServiceFilter
{
public:
    DateRangeFilter();
    virtual ~DateRangeFilter();

    void setRange(const QDate& from, const QDate& to);

    virtual bool pass(const HolyService * const holyService) const override;

private:
    QDate m_from;
    QDate m_to;
};

#endif // DATERANGEFILTER_H
