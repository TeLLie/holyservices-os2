#ifndef CSVEXPORTER_H_INCLUDED_25DC4DE7_9838_47A6_9AD6_43AD37728FCF
#define CSVEXPORTER_H_INCLUDED_25DC4DE7_9838_47A6_9AD6_43AD37728FCF

///\class CsvExporter
/// Export into a CSV (comma separated values) format
/// that is read by most spreadsheet applications (Excel, OpenCalc)
///\date 11-2006
///\author Jan 'Kovis' Struhar

#include "IExporter.h"
#include "ReportColumnDescription.h"

#include <QTextStream>
#include <QFile>

class CsvExporter : public IExporter
{
	typedef IExporter super;
public:
    CsvExporter(const QString& encoding, const tColumnDescriptions& columns);
    virtual ~CsvExporter();

    virtual bool init(const QString& fileName, const QString& options) override;
    virtual void write(const HolyService& hs, int options) override;
    virtual void writeBlankLine(const QDate &dt, int options) override;
    virtual void close() override;
	
private:
	/// encloses list items by quotes and inserts CSV separators inbetween
	static QString makeCsvLine(const QStringList& stringData);

	QTextStream m_ts;
	QFile m_file;
	/// define order and selection of columns
	tColumnDescriptions m_columns;
};

#endif

