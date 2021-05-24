/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "XmlExporter.h"

#include "HolyService.h"
#include "Church.h"
#include "Priest.h"
#include "Client.h"
#include "ReportColumnDescription.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QCoreApplication>
#include <QtDebug>

XmlExporter::XmlExporter()
{
}

XmlExporter::~XmlExporter()
{
}

bool XmlExporter::init(const QString& fileName, const QString& /*options*/)
{
	m_fileName = fileName;

	// copy XSLT template to the target folder
	{
		// get it from the same folder as .exe resides
		const QString templateName("defaultReport.xslt");
		QFileInfo templateOriginalFi(QDir(QCoreApplication::applicationDirPath()), templateName);
		QFileInfo exportedFi(fileName);
		QFileInfo templateCopyFi(exportedFi.absolutePath(), templateName);

		if (templateOriginalFi.exists())
		{
			QFile xsltFile(templateOriginalFi.absoluteFilePath());

			if (! xsltFile.copy(templateCopyFi.absoluteFilePath()))
			{
				qDebug() << "Cannot copy " << templateOriginalFi.absoluteFilePath() <<
					" to " << templateCopyFi.absoluteFilePath();
			}
		}
		else
		{
			qDebug() << "Cannot find the template file " << templateOriginalFi.absoluteFilePath();
		}
	}

	// create DOM document
	QDomNode encodingPi = m_doc.createProcessingInstruction("xml", 
		"version=\"1.0\" encoding=\"UTF-8\"");
	m_doc.appendChild(encodingPi);

	// stylesheet pi
	QDomNode stylesheetPi = m_doc.createProcessingInstruction("xml-stylesheet", 
		"type=\"text/xsl\" href=\"defaultReport.xslt\"");
	m_doc.appendChild(stylesheetPi);

	QDomNode root = m_doc.createElement("HolyServicesOutput");

	// reference to XSLT template
	
	m_doc.appendChild(root);

	return (! m_doc.isNull());
}

void XmlExporter::write(const HolyService& hs, int /*options*/)
{
	if (! m_doc.isNull() && (! m_doc.documentElement().isNull()))
	{
		QDomElement newLine = m_doc.createElement("OutputService");

		newLine.setAttribute("StartWeekDay", ReportColumnDescription::columnData(
            ReportColumnDescription::ServiceWeekday, &hs, holidayProvider()));
        newLine.setAttribute("StartDateDM", ReportColumnDescription::columnData(
            ReportColumnDescription::ServiceStartDate, &hs, holidayProvider()));
        newLine.setAttribute("StartDateShort", ReportColumnDescription::columnData(
            ReportColumnDescription::ServiceStartDateShort, &hs, holidayProvider()));
        newLine.setAttribute("StartTimeHM", ReportColumnDescription::columnData(
            ReportColumnDescription::ServiceStartTime, &hs, holidayProvider()));
		newLine.setAttribute("EndDateDM", ReportColumnDescription::columnData(
            ReportColumnDescription::ServiceEndDate, &hs, holidayProvider()));
		newLine.setAttribute("EndTimeHM", ReportColumnDescription::columnData(
            ReportColumnDescription::ServiceEndTime, &hs, holidayProvider()));
		newLine.setAttribute("Priest", ReportColumnDescription::columnData(
            ReportColumnDescription::PriestName, &hs, holidayProvider()));
		newLine.setAttribute("Church", ReportColumnDescription::columnData(
            ReportColumnDescription::ChurchName, &hs, holidayProvider()));
		newLine.setAttribute("Client", ReportColumnDescription::columnData(
            ReportColumnDescription::ClientName, &hs, holidayProvider()));
		newLine.setAttribute("ClientAddress", ReportColumnDescription::columnData(
            ReportColumnDescription::ClientAddress, &hs, holidayProvider()));
        newLine.setAttribute("Intention", ReportColumnDescription::columnData(
            ReportColumnDescription::Intention, &hs, holidayProvider()));
        newLine.setAttribute("Stipend", ReportColumnDescription::columnData(
            ReportColumnDescription::Stipend, &hs, holidayProvider()));

		m_doc.documentElement().appendChild(newLine);
	}
}

void XmlExporter::close()
{
	if (! m_doc.isNull() && (! m_doc.documentElement().isNull()) &&
		(! m_fileName.isEmpty()))
	{
		QFile f(m_fileName);

		if (f.open(QIODevice::WriteOnly))
		{
			QTextStream ts(&f);

			ts.setCodec("UTF-8");
			ts << m_doc.toString(2);
		}
		m_doc.clear();
	}
}

