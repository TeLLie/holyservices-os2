#pragma once

///\class ServiceCalendarWidget
///\brief Main widget keeping the filters over holy services and all the calendar cells
///
///\date 10-2006
///\author Jan 'Kovis' Struhar

#include "../common/IdTag.h"
#include "UserSettings.h"

#include <QWidget>
#include <QVector>

class IServiceFilter;
class IHolidays;
class ServiceMonthWidget;
class QTabWidget;

class ServiceCalendarWidget : public QWidget
{
	typedef QWidget super;

	Q_OBJECT
public:
	enum EFilterOptions { ALL, NOT_ASSIGNED, ASSIGNED, PARTICULAR };
	
    ServiceCalendarWidget(QWidget *parent);
    virtual ~ServiceCalendarWidget(void);

    void setViewType(UserSettings::MainViewType aType);
	void setYear(const int year);

	void setPriestFilter(const EFilterOptions option, const IdTag& priestId);
	void setChurchFilter(const EFilterOptions option, const IdTag& churchId);
	void propagateFilter();
	IServiceFilter* currentFilter();

	void updateAllOccupied();

    void refreshHolidayProvider();
    IHolidays* holidayProvider() const;

private slots:
    void onFontIncreaseChange(int newIncrease);

private:
    static const int SEASONS_IN_YEAR;
    static const int MONTHS_IN_SEASON;

	typedef QVector<ServiceMonthWidget*> tMonthWidgets;

	tMonthWidgets m_monthWidgets;
    QTabWidget* m_trimesterTabs = nullptr;

    EFilterOptions m_priestOptions;
    IdTag m_priestId;
    EFilterOptions m_churchOptions;
    IdTag m_churchId;

    IServiceFilter* m_filter = nullptr;
    IHolidays* m_holidayProvider = nullptr;

    int m_year = 0;

    /// after set*Filter() create the desired filter
	///\todo cover all EFilterOptions combination, a.t.m. works only particular and ALL
	void createFilter();

    void initializeHolidayProvider();
    ServiceMonthWidget* createMonthWidget(int month, int season);
    QWidget* createSettingsScreen();
    void adaptFontSize();
};
