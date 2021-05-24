/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "UserSettings.h"

#include "ChurchIndexing.h"
#include "PriestIndexing.h"

#include "Priest.h"
#include "Church.h"

#include <QStandardPaths>
#include <QDir>

UserSettings UserSettings::m_singleton;

//@{
///\name Registry keys
const QString yearKey("Year");
const QString priestKey("Priest");
const QString churchKey("Church");
const QString mruKey("RecentlyUsedFiles"); ///< most recently used files
const QString reportColumnsKey("ReportColumns");
const QString googleAccountKey("GoogleAccount");
const QString assignedCriteriaKey("AssignedCriteria");
const QString mainViewKey("MainView");
const QString fontPointIncreaseKey("FontPointIncrease");
//@}

UserSettings::UserSettings() : m_store("Kovis", "HolyServices")
{
	m_year = QDate::currentDate().year();
}

UserSettings::~UserSettings() {}

UserSettings& UserSettings::get()
{
	if (! m_singleton.m_initialized)
	{
		m_singleton.read();
		m_singleton.m_initialized = true;
	}
	return m_singleton;
}

QString UserSettings::defaultFileSuffix()
{
	return "holyservices";
}

void UserSettings::read()
{
	m_year = m_store.value(yearKey, QVariant(QDate::currentDate().year())).toInt();
	m_recentlyUsedFiles = m_store.value(mruKey).toStringList();

	QString str;
	
	str = m_store.value(priestKey).toString();
	if (! str.isEmpty())
	{
		m_priestId = IdTag::createFromString(str);
	}

	str = m_store.value(churchKey).toString();
	if (! str.isEmpty())
	{
		m_churchId = IdTag::createFromString(str);
	}

    m_googleAccount = m_store.value(googleAccountKey).toString();

    m_assignedCriteria = ClientIsAssigned;
    if (m_store.value(assignedCriteriaKey).toInt() == IntentionIsFilled)
    {
        m_assignedCriteria = IntentionIsFilled;
    }

    m_mainView = EntireYear;
    if (m_store.value(mainViewKey).toInt() == Trimesters)
    {
        m_mainView = Trimesters;
    }

    m_fontPointIncrease = m_store.value(fontPointIncreaseKey, 0).toInt();

	// read the columns
	//m_reportColumns = 
	{
		// prepare default contents
		QVariantList defaultColumns;

		{
			ReportColumnDescription aDesc;

			aDesc.setColumnType(ReportColumnDescription::ServiceStartDateTime);
			defaultColumns.append(aDesc.toVariantMap());

			aDesc.setColumnType(ReportColumnDescription::ChurchName);
			defaultColumns.append(aDesc.toVariantMap());

			aDesc.setColumnType(ReportColumnDescription::Intention);
			defaultColumns.append(aDesc.toVariantMap());
		}

		const QVariant storedValue = m_store.value(reportColumnsKey, defaultColumns);
		Q_ASSERT(storedValue.isNull() || storedValue.type() == QVariant::List);
		const QVariantList columnList = storedValue.toList();

		m_reportColumns.clear();

		foreach(const QVariant& columnItem, columnList)
		{
			Q_ASSERT(columnItem.type() == QVariant::Map);
			const QVariantMap columnItemMap = columnItem.toMap();

			if (! columnItemMap.isEmpty())
			{
				ReportColumnDescription columnDescription;

				if (columnDescription.fromVariantMap(columnItemMap))
				{
					m_reportColumns.append(columnDescription);
				}
			}
		}
    }
}

void UserSettings::write()
{
	m_store.setValue(yearKey, m_year);
	m_store.setValue(priestKey, m_priestId.toString());
	m_store.setValue(churchKey, m_churchId.toString());
	m_store.setValue(mruKey, m_recentlyUsedFiles);
    m_store.setValue(googleAccountKey, m_googleAccount);
    m_store.setValue(assignedCriteriaKey, m_assignedCriteria);
    m_store.setValue(mainViewKey, m_mainView);
    m_store.setValue(fontPointIncreaseKey, m_fontPointIncrease);

	// store the report columns
	{
		QVariantList columnList;

		foreach (const ReportColumnDescription& colDesc, m_reportColumns)
		{
			columnList.append(colDesc.toVariantMap());
		}
		m_store.setValue(reportColumnsKey, columnList);
    }
}

QString UserSettings::appSharedDataPath()
{
    QString sharedAppDataFolder;
#ifdef Q_OS_WIN
    sharedAppDataFolder = windowsAppDataFolder();
    sharedAppDataFolder += QDir::separator();
    sharedAppDataFolder += "HolyServices";
#else
    sharedAppDataFolder = "/usr/share/HolyServices";
#endif
    return sharedAppDataFolder;
}

#ifdef Q_OS_WIN
#include <shlobj.h>
#include <wchar.h>
#endif

QString UserSettings::windowsAppDataFolder()
{
    // sh folder api calls
#ifdef Q_OS_WIN
    wchar_t pathBuf[MAX_PATH];

    if (SHGetSpecialFolderPath(0, pathBuf, CSIDL_APPDATA, false))
    {
        return QString::fromWCharArray(pathBuf);
    }
#endif

    return QString();
}

QString UserSettings::userConfigFolder()
{
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
}

UserSettings::MainViewType UserSettings::mainView() const
{
    return m_mainView;
}

void UserSettings::setMainView(MainViewType mainView)
{
    m_mainView = mainView;
}

QString UserSettings::googleAccount() const
{
    return m_googleAccount;
}

void UserSettings::setGoogleAccount(const QString &googleAccount)
{
    m_googleAccount = googleAccount;
}

UserSettings::AssignedCriteria UserSettings::assignedCriteria() const
{
    return m_assignedCriteria;
}

void UserSettings::setAssignedCriteria(UserSettings::AssignedCriteria criteria)
{
    m_assignedCriteria = criteria;
}

bool UserSettings::isClientImportant() const
{
    return (m_assignedCriteria == ClientIsAssigned);
}

void UserSettings::setDefaultPriest(Priest* pPriest)
{
    m_priestId = (pPriest) ? pPriest->getId() : IdTag();
}

Priest* UserSettings::defaultPriest() const
{
    return (m_priestId.isValid()) ?
		PriestIndexing::allSet().find(m_priestId) : nullptr;
}

void UserSettings::setDefaultChurch(Church* pChurch)
{
	m_churchId = (pChurch) ? pChurch->getId() : IdTag();
}

Church* UserSettings::defaultChurch() const
{
	return (m_churchId.isValid()) ? 
		ChurchIndexing::allSet().find(m_churchId) : nullptr;
}

void UserSettings::setDefaultYear(int year)
{
	m_year = year;
}

int UserSettings::defaultYear() const
{
	return m_year;
}

void UserSettings::addRecentlyUsedFile(const QString& usedFile)
{
	removeRecentlyUsedFile(usedFile);
	m_recentlyUsedFiles.prepend(usedFile);
}

void UserSettings::removeRecentlyUsedFile(const QString& aFile)
{
	m_recentlyUsedFiles.removeAll(aFile);
}

const QStringList& UserSettings::recentlyUsedFiles() const
{
	return m_recentlyUsedFiles;
}

tColumnDescriptions UserSettings::reportColumns() const
{
    return m_reportColumns;
}

bool UserSettings::someColumnIsVisible() const
{
    return std::any_of(m_reportColumns.cbegin(), m_reportColumns.cend(),
        [](const ReportColumnDescription& desc) { return desc.visible(); });
}

void UserSettings::setReportColumns(const tColumnDescriptions& newList)
{
	m_reportColumns = newList;
}

int UserSettings::fontPointIncrease() const
{
    return m_fontPointIncrease;
}

void UserSettings::setFontPointIncrease(int fontPointIncrease)
{
    m_fontPointIncrease = fontPointIncrease;
}

