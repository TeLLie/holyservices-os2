/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "OpenOfficeExporter.h"

#include <QTextDocumentWriter>

OpenOfficeExporter::OpenOfficeExporter(const tColumnDescriptions& columns) : super(columns)
{
}

OpenOfficeExporter::~OpenOfficeExporter()
{
}

void OpenOfficeExporter::close()
{
    QTextDocumentWriter writer(fileNameToExport(), "ODF");

    writer.write(exportedDocument());
}
