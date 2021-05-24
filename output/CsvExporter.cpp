/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "CsvExporter.h"

#include "HolyService.h"
#include "Church.h"
#include "Priest.h"
#include "Client.h"

#include <QString>
#include <QFile>
#include <QStringList>

CsvExporter::CsvExporter(const QString& encoding, const tColumnDescriptions& columns) 
{
	m_ts.setCodec(qPrintable(encoding));
	m_columns = columns;
}

CsvExporter::~CsvExporter()
{
}

bool CsvExporter::init(const QString& fileName, const QString& /*options*/)
{
	m_file.setFileName(fileName);

	bool retVal = m_file.open(QIODevice::WriteOnly);
	if (retVal)
	{
		m_ts.setDevice(&m_file);

		QStringList headerTexts;

		foreach(const ReportColumnDescription& column, m_columns)
		{
			if (column.visible())
			{
				headerTexts.append(ReportColumnDescription::columnHeader(column.columnType()));
			}
		}
		// enclose the cells by quote characters to improve spreadsheet import
        m_ts << makeCsvLine(headerTexts) << endl;
	}
	return retVal;
}

void CsvExporter::write(const HolyService &hs, int /*options*/)
{
    if (! m_ts.device()) return;

    QStringList dataLine;
    foreach(const ReportColumnDescription& column, m_columns)
    {
        if (column.visible())
        {
            dataLine.append(ReportColumnDescription::columnData(
                                column.columnType(), &hs, holidayProvider()));
        }
    }
    // enclose the cells by quote characters to improve spreadsheet import
    m_ts << makeCsvLine(dataLine) << endl;
}

void CsvExporter::writeBlankLine(const QDate &dt, int /*options*/)
{
    QStringList dataLine;
    foreach(const ReportColumnDescription& column, m_columns)
    {
        if (column.visible())
        {
            dataLine.append(ReportColumnDescription::columnData(
                                column.columnType(), dt, holidayProvider()));
        }
    }
    // enclose the cells by quote characters to improve spreadsheet import
    m_ts << makeCsvLine(dataLine) << endl;
}

void CsvExporter::close()
{
	if (m_ts.device())
	{
		m_ts.device()->close();
	}
}

QString CsvExporter::makeCsvLine(const QStringList& stringData)
{
	static const QChar csvDelimiter(';');
	static const QChar quote('"');
	static const QString csvSeparator(QString(quote) + csvDelimiter + quote);

	return quote + stringData.join(csvSeparator) + quote;
}


