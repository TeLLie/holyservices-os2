#ifndef XMLEXPORTER_H_INCLUDED_B188FB2D_084C_446F_9FCE_DE5DDEB0A649
#define XMLEXPORTER_H_INCLUDED_B188FB2D_084C_446F_9FCE_DE5DDEB0A649

///\class XmlExporter
/// Exporter buffers incoming records to a DOM document,
/// writes it to a file on closing. XML references XSLT template
/// defaultReport.xslt thus should be correctly displayed in a web
/// browser
///\date 11-2006
///\author Jan 'Kovis' Struhar
#include "IExporter.h"

#include <QString>
#include <QDomDocument>

class XmlExporter : public IExporter
{
	typedef IExporter super;
public:
    XmlExporter();
    virtual ~XmlExporter();

	virtual bool init(const QString& fileName, const QString& options);
	virtual void write(const HolyService& hs, int options);
	virtual void close();	
	
private:
	QDomDocument m_doc;
	QString m_fileName;
};

#endif

