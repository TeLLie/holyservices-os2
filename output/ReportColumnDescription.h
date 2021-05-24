#ifndef REPORTCOLUMNDESCRIPTION_H_INCLUDED_FA55B66A_9D18_4A42_BA6F_A11EC9578715
#define REPORTCOLUMNDESCRIPTION_H_INCLUDED_FA55B66A_9D18_4A42_BA6F_A11EC9578715

///\class ReportColumnDescription
/// Describes one column of exported holy service. Width is not used a.t.m.
/// Provides general support for all exporters because it unifies 
/// the format of data cells and column names throughout application
///\date 4-2008
///\author Jan 'Kovis' Struhar

#include <QList>
#include <QVariant>

class HolyService;
class IHolidays;
class QString;

class ReportColumnDescription
{
public:
	/// Do not change the order of enum members or you
	/// shall screw the stored report definitions
	enum EType { PriestName = 0, ChurchName, Intention, 
		ServiceWeekday, 
		ServiceStartDate, ServiceStartTime, ServiceStartDateTime,
		ServiceEndDate, ServiceEndTime, ServiceEndDateTime,
        ClientName, ClientAddress, HolidayText, ChurchAbbreviation,
        ServiceStartDateShort,
        Stipend,
        ColumnCount };

    static QString columnData(const EType columnType, const HolyService* const, const IHolidays* const holidays);
    static QString columnData(const EType columnType, const QDate &dt, const IHolidays* const holidays);
    static QString columnHeader(EType columnType);

    ReportColumnDescription();
    virtual ~ReportColumnDescription();

    EType columnType() const;
    void setColumnType(EType newType);

	/// report visibility flag
    bool visible() const;
    void setVisible(bool newVal);

	/// persistence to/from registry or ini file
	QVariantMap toVariantMap() const;
	bool fromVariantMap(const QVariantMap&);

private:
	EType m_columnType;
	/// should the column be visible in the export?
	bool m_visible;
};

typedef QList<ReportColumnDescription> tColumnDescriptions;

#endif
