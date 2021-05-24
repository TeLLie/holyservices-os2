#ifndef HTMLEXPORTER_H_INCLUDED_A512F5CD_FBFA_4B1C_9A41_0475797544DE
#define HTMLEXPORTER_H_INCLUDED_A512F5CD_FBFA_4B1C_9A41_0475797544DE

///\class HtmlExporter
/// Direct HTML export (produces common HTML file, no XML to be XSLT transformed
/// as CXmlExporter does)
///\date 3-2008
///\author Jan 'Kovis' Struhar
#include "DocumentExporter.h"

#include "ReportColumnDescription.h"

class HtmlExporter : public DocumentExporter
{
	typedef DocumentExporter super;
public:
    HtmlExporter(const tColumnDescriptions&);
    virtual ~HtmlExporter();

	virtual void close();	
};

#endif
