#ifndef PRINTEREXPORTER_H_INCLUDED_5F7DA811_D3A2_4D21_9923_29D8EAFB0E2E
#define PRINTEREXPORTER_H_INCLUDED_5F7DA811_D3A2_4D21_9923_29D8EAFB0E2E

///\class PrinterExporter
/// Direct print on the printer
///\date 3-2008
///\author Jan 'Kovis' Struhar

#include "DocumentExporter.h"
#include "ReportColumnDescription.h"

#include <QPrinter>

class PrinterExporter : public DocumentExporter
{
	typedef DocumentExporter super;
public:
    PrinterExporter(QPrinter::OutputFormat outputFormat, const tColumnDescriptions&);
    virtual ~PrinterExporter();

	virtual void close();	

private:
	const QPrinter::OutputFormat m_format;
};

#endif
