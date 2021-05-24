/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "HtmlExporter.h"

#include <QFile>
#include <QTextStream>

HtmlExporter::HtmlExporter(const tColumnDescriptions& columns) : super(columns)
{
}

HtmlExporter::~HtmlExporter()
{
}

void HtmlExporter::close()
{
//	qDebug() << exportedDocument()->toPlainText();

	QFile f(fileNameToExport());

	if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QTextStream ts(&f);
	
		ts.setCodec("utf-8");
		ts << exportedDocument()->toHtml("utf-8");

		f.close();
	}
}
