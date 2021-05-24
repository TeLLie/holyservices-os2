/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ImportCsv.h"

#include "ClientIndexing.h"
#include "PriestIndexing.h"
#include "ChurchIndexing.h"
#include "HolyServiceIndexing.h"

#include <QFile>
#include <QTextStream>
#include <assert.h>
#include <memory>


ImportCsv::ImportCsv()
{
}

ImportCsv::~ImportCsv()
{
}

bool ImportCsv::import(const QString& churchFile, const QString& priestFile,
		const QString& clientFile, const QString& serviceFile, QString& report)
{
	report.clear();
	m_priests.clear(); m_churches.clear(); m_clients.clear(); m_services.clear();

	bool retVal = importCsv(churchFile, m_churches);
	if (retVal == false)
	{
		report.append(QString("Failed to import %1\n").arg(churchFile));
	}
	retVal = importCsv(priestFile, m_priests);
	if (retVal == false)
	{
		report.append(QString("Failed to import %1\n").arg(priestFile));
	}
	retVal = importCsv(clientFile, m_clients);
	if (retVal == false)
	{
		report.append(QString("Failed to import %1\n").arg(clientFile));
	}
	retVal = importCsv(serviceFile, m_services);
	if (retVal == false)
	{
		report.append(QString("Failed to import %1\n").arg(serviceFile));
	}

	if (retVal)
	{
		retVal = transferToThisProgram(report);
	}
	return retVal;
}


bool ImportCsv::importCsv(const QString& fileName, tCsvImported& data)
{
	// first key is assumed to be key
	QFile f(fileName);

	data.clear();

	if (f.open(QIODevice::ReadOnly))
	{
		QTextStream ts(&f);
		const QChar separator(';');
		QString line, key;
		QStringList value;

		// locale coding assumed

		while (! ts.atEnd())
		{
			line = ts.readLine();
			line.remove('"'); // get rid of text denominator in CSV
			key = line.section(separator, 0, 0);
			value = line.split(separator);	

			// remove the key from values
			if (! value.isEmpty())
			{
				value.removeFirst();
			}

			assert(! data.contains(key));
			data[key] = value;
		}
		f.close();
		return true;
	}
	return false;
}

bool ImportCsv::transferToThisProgram(QString& report)
{
	report.append("\nTransfer starts");

	tCsvToAppKeys clientMap, churchMap, priestMap; // service map is not needed, it is not referred to

	// clients
	for (tCsvImportedConstIt it = m_clients.constBegin(); it != m_clients.constEnd(); ++it)
	{
        std::unique_ptr<Client> newClient(new Client);
		const QStringList& values = it.value();
		const IdTag newId = IdTag::createInitialized();

		newClient->setId(newId);
		newClient->setFirstName(values.at(0));
		newClient->setSurname(values.at(1));
		newClient->setStreet(values.at(4));

		if (ClientIndexing::allSet().addElement(newClient.release()))
		{
			clientMap[it.key()] = newId;
		}
	}

	report.append(QString("\nAdded %1 clients").arg(clientMap.size()));

	// priests
	for (tCsvImportedConstIt it = m_priests.constBegin(); it != m_priests.constEnd(); ++it)
	{
        std::unique_ptr<Priest> newPriest(new Priest);
		const QStringList& values = it.value();
		const IdTag newId = IdTag::createInitialized();

		newPriest->setId(newId);
		newPriest->setFirstName(values.at(1));
		newPriest->setSurname(values.at(2));

        if (PriestIndexing::allSet().addElement(newPriest.release()))
		{
			priestMap[it.key()] = newId;
		}
	}

	report.append(QString("\nAdded %1 priests").arg(priestMap.size()));

	// churchs
	for (tCsvImportedConstIt it = m_churches.constBegin(); it != m_churches.constEnd(); ++it)
	{
        std::unique_ptr<Church> newChurch(new Church);
		const QStringList& values = it.value();
		const IdTag newId = IdTag::createInitialized();

		newChurch->setId(newId);
		newChurch->setName(values.at(0).section(',', 0, 0).trimmed());
		newChurch->setPlace(values.at(0).section(',', 1, 1).trimmed());

        if (ChurchIndexing::allSet().addElement(newChurch.release()))
		{
			churchMap[it.key()] = newId;
		}
	}

	report.append(QString("\nAdded %1 churchs").arg(churchMap.size()));

	// holy services
	int serviceCount = 0;

	for (tCsvImportedConstIt it = m_services.constBegin(); it != m_services.constEnd(); ++it)
	{
        std::unique_ptr<HolyService> newHolyService(new HolyService);
		const QStringList& values = it.value();
		const IdTag newId = IdTag::createInitialized();

		newHolyService->setId(newId);

		const QDate startDate = QDate::fromString(values.at(1), "d.M.yyyy");
		QTime startTime = QTime::fromString(values.at(2), "h:mm:ss");

		if (! startTime.isValid())
		{
			startTime = QTime(10, 0, 0);
		}

		newHolyService->setStartTime(QDateTime(startDate, startTime));

		startTime = startTime.addSecs(3600);
		newHolyService->setEndTime(QDateTime(startDate, startTime));

		newHolyService->setIntention(values.at(3));
		newHolyService->setClientId(clientMap[values.at(4)]);
		newHolyService->setChurchId(churchMap[values.at(5)]);
		newHolyService->setPriestId(priestMap[values.at(6)]);

		HolyService* pConflicting;
		const bool added = HolyServiceIndexing::allSet().
			addElement(newHolyService.release(), pConflicting);
		assert(added);
		if (added)
		{
			++serviceCount;
		}
	}
	report.append(QString("\nAdded %1 holy services").arg(serviceCount));

	return true;
}
