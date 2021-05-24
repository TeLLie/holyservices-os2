#ifndef USERSETTINGS_H_INCLUDED_CB627475_1CBC_4365_ABF3_44D7579D6956
#define USERSETTINGS_H_INCLUDED_CB627475_1CBC_4365_ABF3_44D7579D6956

///\class UserSettings
///\brief User preferences stored in registry
///\date 11-2006
///\author Jan 'Kovis' Struhar

#include "../common/IdTag.h"
#include "../output/ReportColumnDescription.h"
#include <QSettings>
#include <QStringList>

class Priest;
class Church;

class UserSettings
{
public:
	/// singleton accessor
    static UserSettings& get();

	/// globally accessible default file suffix
	static QString defaultFileSuffix();

	/// recently selected priest filtering - nullptr for all priests
	void setDefaultPriest(Priest*);
	Priest* defaultPriest() const;

	/// recently selected church, nullptr means all churches
	void setDefaultChurch(Church*);
	Church* defaultChurch() const;
	
	/// recently selected year in the toolbar
	void setDefaultYear(int year);
	int defaultYear() const;

	/// Most recently used files storage
	void addRecentlyUsedFile(const QString&);
	void removeRecentlyUsedFile(const QString&);
	const QStringList& recentlyUsedFiles() const;

	/// report columns that are contained in exported data
	/// the ints are CReportColumnDescription::EType values
	tColumnDescriptions reportColumns() const;
    bool someColumnIsVisible() const;
	void setReportColumns(const tColumnDescriptions&);

    QString googleAccount() const;
    void setGoogleAccount(const QString &googleAccount);

    enum AssignedCriteria
    {
        ClientIsAssigned = 0,
        IntentionIsFilled // better for cities
    };
    AssignedCriteria assignedCriteria() const;
    void setAssignedCriteria(AssignedCriteria crieria);
    bool isClientImportant() const;

    enum MainViewType
    {
        EntireYear = 0,
        Trimesters
    };
    MainViewType mainView() const;
    void setMainView(MainViewType mainView);

    int fontPointIncrease() const;
    void setFontPointIncrease(int fontPointIncrease);

    /// save the values into registry
	void write();

    static QString appSharedDataPath();

    /// where to reach for the user writable folder with, for instance, user specific holidays?
    static QString userConfigFolder();

private:
    static UserSettings m_singleton;

    QSettings m_store;

    IdTag m_priestId;
    IdTag m_churchId;
    int m_year = 0;
	QStringList m_recentlyUsedFiles;
	/// contains QVariantList of QVariantMaps with CReportColumnDescriptions data
	tColumnDescriptions m_reportColumns;
    QString m_googleAccount;
    AssignedCriteria m_assignedCriteria = ClientIsAssigned;
    MainViewType m_mainView = EntireYear;
    int m_fontPointIncrease = 0;

    bool m_initialized = false;

    UserSettings();
    UserSettings(const UserSettings&);
    virtual ~UserSettings();

	void read();
    static QString windowsAppDataFolder();
};

#endif
