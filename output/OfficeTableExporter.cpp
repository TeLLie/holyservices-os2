/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "OfficeTableExporter.h"

#include <QClipboard>
#include <QMimeData>
#include <QApplication>

OfficeTableExporter::OfficeTableExporter(const tColumnDescriptions& columns) : super(columns)
{
}

OfficeTableExporter::~OfficeTableExporter()
{

}

void OfficeTableExporter::close()
{
    QMimeData *htmlData = new QMimeData;

    htmlData->setHtml(exportedDocument()->toHtml());
    QApplication::clipboard()->setMimeData(htmlData);
}
