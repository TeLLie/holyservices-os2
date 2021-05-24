#ifndef CALENDAREXPORTER_H
#define CALENDAREXPORTER_H

///\class CalendarExporter
/// iCal format export http://tools.ietf.org/html/rfc2445
/// for the common calendar software like KOrganizer, Mozilla Lightning
/// GoogleCalendar or MS OutLook
///\date 9-2009
///\author Jan 'Kovis' Struhar

#include "IExporter.h"
#include <QTextStream>
#include <QFile>
#include <QDateTime>

class CalendarExporter : public IExporter
{
public:
    CalendarExporter();
    virtual ~CalendarExporter();

    virtual bool init(const QString& fileName, const QString& options) override;
    virtual void write(const HolyService& hs, int options) override;
    virtual void close() override;

private:
	/// retrieve a iCal compatible UTC datetime string
	static QString utcString(const QDateTime& dt);

	/// do line folding of lines over 75 characters as required by iCal format
	void writeText(const QString& text);

	QTextStream m_ts;
	QFile m_file;
};

#endif // CALENDAREXPORTER_H
