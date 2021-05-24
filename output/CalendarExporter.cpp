/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "CalendarExporter.h"

#include "HolyService.h"
#include "Church.h"
#include "Client.h"

CalendarExporter::CalendarExporter()
{
	m_ts.setCodec("UTF-8"); // RFC requirement
}

CalendarExporter::~CalendarExporter()
{
}

bool CalendarExporter::init(const QString& fileName, const QString& /*options*/)
{
	m_file.setFileName(fileName);

	bool retVal = m_file.open(QIODevice::WriteOnly);
	if (retVal)
	{
		m_ts.setDevice(&m_file);

		// write common iCal header
		writeText("BEGIN:VCALENDAR");
		writeText("VERSION:2.0");
		writeText("PRODID:-//kovis/HolyServices");
	}
	return retVal;
}

void CalendarExporter::write(const HolyService &hs, int /*options*/)
{
	writeText("BEGIN:VEVENT");
	writeText(QString("CREATED:%1").arg(utcString(hs.created())));
	writeText(QString("LAST-MODIFIED:%1").arg(utcString(hs.lastModified())));
	writeText(QString("UID:%1").arg(hs.getId().toString()));
	writeText(QString("DTSTAMP:%1").arg(utcString(QDateTime::currentDateTime())));

	const Church *church = hs.getChurch();
	{
		QString summary;
		if (church)
		{
			summary = QObject::tr("Holy service in %1").arg(church->place());
		}
		else
		{
			summary = QObject::tr("Holy service");
		}
		writeText(QString("SUMMARY:%1").arg(summary));
	}

	const QString location = (church) ?
		church->place() : QObject::tr("Unknown location");
	writeText(QString("LOCATION:%1").arg(location));

	writeText(QString("DTSTART:%1").arg(utcString(hs.startTime())));
	writeText(QString("DTEND:%1").arg(utcString(hs.endTime())));
	writeText(QString("DESCRIPTION:%1").arg(hs.intention()));

	if (hs.getClient())
	{
		writeText(QString("COMMENT:%1").arg(hs.getClient()->toString(Client::NameSurnameStreet)));
	}
	writeText("END:VEVENT");
}

void CalendarExporter::close()
{
	if (m_ts.device() == nullptr) return;

	// write common iCal closing sequence
	writeText("END:VCALENDAR");

	m_ts.device()->close();
}

void CalendarExporter::writeText(const QString& text)
{
	if (text.trimmed().isEmpty()) return;

	// fold the line after 60 unicode characters as reasonable
	// size so as not to exceed 75 characters required by specs
	const int maxCharsOnLine = 60;
	QString textCopy(text);

	textCopy.replace('\n', "\\n"); // newlines treated according to RFC

	do
	{
		m_ts << textCopy.left(maxCharsOnLine) << "\n";
		textCopy.remove(0, maxCharsOnLine);
		if (! textCopy.isEmpty())
		{
			textCopy.prepend("\t"); // any whitechar would do
		}
	}
	while (! textCopy.isEmpty());
}

QString CalendarExporter::utcString(const QDateTime& dt)
{
	return dt.toUTC().toString("yyyyMMddThhmmssZ");
}

