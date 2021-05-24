/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "DocumentExporter.h"

#include "HolyService.h"

#include <QtDebug>
#include <QTextCursor>
#include <QTextTable>
#include <QTextTableCell>

DocumentExporter::DocumentExporter(const tColumnDescriptions& cols)
{
	m_document.reset(new QTextDocument);
	m_document->setMetaInformation(QTextDocument::DocumentTitle, 
		QObject::tr("Export of holy services"));
	m_columns = cols;
}

DocumentExporter::~DocumentExporter()
{
}

bool DocumentExporter::init(const QString& fileName, const QString& /*options*/)
{
	m_document->clear();
	m_fileName = fileName;

	{
		QTextCursor cursor(m_document->rootFrame());
		
		// open the table 
		QTextTableFormat tableFormat;
		
		tableFormat.setHeaderRowCount(1);

		// find out number of visible columns
		int columnCount = 0;

		foreach(const ReportColumnDescription& desc, m_columns)
		{
			if (desc.visible())
			{
				columnCount++;
			}
		}

		m_table.reset(cursor.insertTable(1, columnCount, tableFormat));
	}

	// fill the header row
	int columnIndex = 0;
	foreach (const ReportColumnDescription& desc, m_columns)
	{
		if (desc.visible())
		{
			QTextTableCell cell = m_table->cellAt(0, columnIndex);

			// set the background
			QTextCharFormat format = cell.format(); 
			format.setBackground(Qt::lightGray); 
			cell.setFormat(format); 

			// and text
			QTextCursor cellCursor = cell.firstCursorPosition();

			cellCursor.insertText(ReportColumnDescription::columnHeader(desc.columnType()));

			++columnIndex;
		}
	}
	
    return (m_table != 0);
}

void DocumentExporter::write(const HolyService& hs, int /*options*/)
{
	// insert new row
	m_table->insertRows(m_table->lastPosition(), 1);

	// and populate it with the data
	const int lastRow = m_table->rows() - 1;
	int columnIndex = 0;

	foreach (const ReportColumnDescription& desc, m_columns)
	{
		if (desc.visible())
		{
			QTextTableCell cell = m_table->cellAt(lastRow, columnIndex);
			QTextCursor cellCursor = cell.firstCursorPosition();

			cellCursor.insertText(ReportColumnDescription::columnData(
                desc.columnType(), &hs, holidayProvider()));
			++columnIndex;
		}
    }
}

void DocumentExporter::writeBlankLine(const QDate &dt, int /*options*/)
{
    // insert new row
    m_table->insertRows(m_table->lastPosition(), 1);

    // and populate it with the data
    const int lastRow = m_table->rows() - 1;
    int columnIndex = 0;

    foreach (const ReportColumnDescription& desc, m_columns)
    {
        if (desc.visible())
        {
            QTextTableCell cell = m_table->cellAt(lastRow, columnIndex);
            QTextCursor cellCursor = cell.firstCursorPosition();

            cellCursor.insertText(ReportColumnDescription::columnData(
                desc.columnType(), dt, holidayProvider()));
            ++columnIndex;
        }
    }
}

const QString& DocumentExporter::fileNameToExport() const
{
	return m_fileName;
}

QTextDocument* DocumentExporter::exportedDocument() const
{
	return m_document.get();
}
