/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "PrinterExporter.h"

#include "../gui/WaitingCursor.h"

#include <QApplication>
#include <QPrintDialog>

PrinterExporter::PrinterExporter(QPrinter::OutputFormat outputFormat,
	const tColumnDescriptions& columns) : super(columns), m_format(outputFormat)
{
}

PrinterExporter::~PrinterExporter()
{
}

void PrinterExporter::close()
{
	QPrinter printer(QPrinter::HighResolution);
	// find out the paper size and orientation for native printer
	QPrintDialog printDialog(&printer, QApplication::activeWindow());
    
	printDialog.setWindowTitle(QObject::tr("Report printing"));
    
	if (printDialog.exec() == QDialog::Accepted)
	{
		const WaitingCursor sandHours;

		if (m_format != QPrinter::NativeFormat)
		{
			printer.setOutputFileName(fileNameToExport());
		}
		printer.setOutputFormat(m_format);

		exportedDocument()->print(&printer);
	}
}

