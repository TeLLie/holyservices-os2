#ifndef DOCUMENTEXPORTER_H_INCLUDED_2F71E95C_7534_4E7A_8261_64FB67EA3448
#define DOCUMENTEXPORTER_H_INCLUDED_2F71E95C_7534_4E7A_8261_64FB67EA3448

///\class DocumentExporter
/// Exporter to QTextDocument class that is able to render self to 
/// native printer, PDF, PostScript, HTML, plain text. The particular export
/// type is implemented in subclasses. Main concern of this class is to create 
/// the document and put the proper columns and contents into the document
///\date 3-2008
///\author Jan 'Kovis' Struhar

#include "IExporter.h"
#include "ReportColumnDescription.h"

#include <QTextDocument>
#include <memory>

class QTextTable;

class DocumentExporter : public IExporter
{
	typedef IExporter super;
public:
    DocumentExporter(const tColumnDescriptions&);
    virtual ~DocumentExporter();

    virtual bool init(const QString& fileName, const QString& options) override;
    virtual void write(const HolyService& hs, int options) override;
    virtual void writeBlankLine(const QDate &dt, int options) override;

	// intentionally not implemented
	// virtual void close();	

protected:
	const QString& fileNameToExport() const;
	QTextDocument* exportedDocument() const;

private:
	std::unique_ptr<QTextDocument>m_document;
	std::unique_ptr<QTextTable> m_table;
	QString m_fileName;
	tColumnDescriptions m_columns;
};

#endif
