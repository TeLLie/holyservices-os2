/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "HistoryData.h"

#include <stdlib.h>

#ifdef Q_OS_WIN32
#include <windows.h>
#include <Lmcons.h>
#endif

HistoryData::HistoryData(void)
{
}

HistoryData::~HistoryData(void)
{
}

QDateTime HistoryData::created() const
{
	return m_created;
}

void HistoryData::setCreated(const QDateTime& dt)
{
	m_created = dt;
}

QString HistoryData::creator() const
{
	return m_creator;
}

void HistoryData::setCreator(const QString& s)
{
	m_creator = s;
}

QDateTime HistoryData::lastModified() const
{
	return m_lastModified;
}

void HistoryData::setLastModified(const QDateTime& dt)
{
	m_lastModified = dt;
}

QString HistoryData::lastModificator() const
{
	return m_lastModificator;
}

void HistoryData::setLastModificator(const QString& s)
{
	m_lastModificator = s;
}

void HistoryData::modifyNow()
{
	m_lastModified = QDateTime::currentDateTime();
	m_lastModificator = currentUserAndComputer();
}

void HistoryData::createNow()
{
	m_created = QDateTime::currentDateTime();
	m_creator = currentUserAndComputer();
}


QString HistoryData::currentUserAndComputer()
{
	return currentUser() + '@' + currentComputer();
}

QString HistoryData::currentUser()
{
#ifdef Q_OS_WIN32
	DWORD bufLen = UNLEN + 1;
	wchar_t buf[UNLEN + 1];
	
	if (GetUserNameW(buf, &bufLen))
	{
		return QString::fromUtf16(reinterpret_cast<ushort*>(buf));
	}
	return QString();
#else
	return QString::fromUtf8(getenv("USER"));
#endif
}

QString HistoryData::currentComputer()
{
#ifdef Q_OS_WIN32
	DWORD bufLen = MAX_COMPUTERNAME_LENGTH + 1;
	wchar_t buf[MAX_COMPUTERNAME_LENGTH + 1];
	
	if (GetComputerNameW(buf, &bufLen))
	{
		return QString::fromUtf16(reinterpret_cast<ushort*>(buf));
	}
	return QString();
#else
	return QString::fromUtf8(getenv("HOST"));
#endif
}


