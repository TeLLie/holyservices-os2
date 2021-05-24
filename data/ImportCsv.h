#ifndef IMPORTCSV_H_INCLUDED_67CCFE1B_CAEC_4E59_9285_ED6DEE8AAFCD
#define IMPORTCSV_H_INCLUDED_67CCFE1B_CAEC_4E59_9285_ED6DEE8AAFCD

///\class ImportCsv
///\brief Import from legacy holy service application
///\date 11-2006
///\author Jan 'Kovis' Struhar

#include "../common/IdTag.h"

#include <QMap>
#include <QStringList>

class ImportCsv
{
public:
    ImportCsv();
    virtual ~ImportCsv();

	bool import(const QString& churchFile, const QString& priestFile,
		const QString& clientFile, const QString& serviceFile, QString& report);
private:
	typedef QMap<QString, QStringList> tCsvImported;
	typedef tCsvImported::const_iterator tCsvImportedConstIt;
	typedef QMap<QString, IdTag> tCsvToAppKeys;

	tCsvImported m_priests, m_churches, m_clients, m_services;

	static bool importCsv(const QString& fileName, tCsvImported& data);

	bool transferToThisProgram(QString& report);
};

#endif
