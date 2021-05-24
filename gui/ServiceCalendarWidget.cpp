/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ServiceCalendarWidget.h"

#include "ServiceMonthWidget.h"
#include "AndFilter.h"
#include "PriestFilter.h"
#include "ChurchFilter.h"
#include "HolidayComposition.h"
#include "HolidayListModel.h"
#include "UserSettings.h"
#include "WaitingCursor.h"

#include <QTabWidget>
#include <QGridLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QtDebug>

const int ServiceCalendarWidget::SEASONS_IN_YEAR = 4;
const int ServiceCalendarWidget::MONTHS_IN_SEASON = 3;

ServiceCalendarWidget::ServiceCalendarWidget(QWidget *parent) : super(parent)
{
    initializeHolidayProvider();
	m_priestOptions = m_churchOptions = ALL;
	m_filter = nullptr;
}

ServiceCalendarWidget::~ServiceCalendarWidget(void)
{
    delete m_holidayProvider;
}

void ServiceCalendarWidget::setViewType(UserSettings::MainViewType aType)
{
    const auto hadContents = (! m_monthWidgets.isEmpty());
    qDeleteAll(m_monthWidgets);
    m_monthWidgets.clear();
    delete m_trimesterTabs; m_trimesterTabs = nullptr;
    delete layout();

    switch (aType)
    {
    case UserSettings::EntireYear:
        {
            auto layout = new QGridLayout(this);
            for (int season = 0; season < SEASONS_IN_YEAR; ++season)
            {
                for (int month = 0; month < MONTHS_IN_SEASON; ++month)
                {
                    auto pMonthWidget = createMonthWidget(month, season);
                    layout->addWidget(pMonthWidget, month, season);

                    m_monthWidgets.push_back(pMonthWidget);
                }
            }
        }
        break;
    case UserSettings::Trimesters:
        {
            auto layout = new QVBoxLayout(this);
            m_trimesterTabs = new QTabWidget(this);

            for (int season = 0; season < SEASONS_IN_YEAR; ++season)
            {
                auto trimesterWidget = new QWidget();
                auto trimesterLayout = new QHBoxLayout(trimesterWidget);
                for (int month = 0; month < MONTHS_IN_SEASON; ++month)
                {
                    auto pMonthWidget = createMonthWidget(month, season);
                    trimesterLayout->addWidget(pMonthWidget);

                    m_monthWidgets.push_back(pMonthWidget);
                }

                QString trimesterLabel;
                switch(season)
                {
                case 0: trimesterLabel = tr("January-March"); break;
                case 1: trimesterLabel = tr("April-June"); break;
                case 2: trimesterLabel = tr("July-September"); break;
                case 3: trimesterLabel = tr("October-December"); break;
                }
                m_trimesterTabs->addTab(trimesterWidget, trimesterLabel);
            }

            adaptFontSize();
            m_trimesterTabs->addTab(createSettingsScreen(), QPixmap(":images/exec.png"), tr("Settings"));
            layout->addWidget(m_trimesterTabs);
        }
        break;
    }

    if (hadContents && (m_year > 0))
    {
        setYear(m_year);
    }
}

ServiceMonthWidget* ServiceCalendarWidget::createMonthWidget(int month, int season)
{
    auto pMonthWidget = new ServiceMonthWidget(this);
    pMonthWidget->setMonth((month + 1) + (season * MONTHS_IN_SEASON), 2006);
    pMonthWidget->setHolidays(m_holidayProvider);
    return pMonthWidget;
}

QWidget *ServiceCalendarWidget::createSettingsScreen()
{
    auto page = new QWidget(nullptr);
    auto formLayout = new QFormLayout(page);
    auto fontSizeBox = new QSpinBox(page);
    fontSizeBox->setSuffix(tr(" pt"));
    fontSizeBox->setRange(0, 40);
    fontSizeBox->setMaximumWidth(80);
    fontSizeBox->setValue(UserSettings::get().fontPointIncrease());
    fontSizeBox->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    connect(fontSizeBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ServiceCalendarWidget::onFontIncreaseChange);
    formLayout->addRow(tr("Calendar font size increase:"), fontSizeBox);
    return page;
}

void ServiceCalendarWidget::onFontIncreaseChange(int newIncrease)
{
    UserSettings::get().setFontPointIncrease(newIncrease);
    adaptFontSize();
}

void ServiceCalendarWidget::adaptFontSize()
{
    auto increasedFont = font();
    increasedFont.setPointSize(increasedFont.pointSize() + UserSettings::get().fontPointIncrease());
    for (auto w : m_monthWidgets)
    {
        w->setFont(increasedFont);
    }
}

void ServiceCalendarWidget::initializeHolidayProvider()
{
    const QString sharedAppDataFolder = UserSettings::appSharedDataPath();
    const QString userWritableConfigFolder = UserSettings::userConfigFolder();
    m_holidayProvider = new HolidayComposition(sharedAppDataFolder, userWritableConfigFolder);
}

void ServiceCalendarWidget::setYear(const int year)
{
    m_year = year;
    if (m_monthWidgets.isEmpty())
        return;

    for (int month = 0; month < (SEASONS_IN_YEAR * MONTHS_IN_SEASON); ++month)
	{
        m_monthWidgets.at(month)->setMonth(month + 1, m_year);
	}
}

void ServiceCalendarWidget::setPriestFilter(const EFilterOptions option, const IdTag& priestId)
{
	m_priestOptions = option;
	m_priestId = priestId;
}

void ServiceCalendarWidget::setChurchFilter(const EFilterOptions option, const IdTag& churchId)
{
	m_churchOptions = option;
	m_churchId = churchId;
}

void ServiceCalendarWidget::propagateFilter()
{
	createFilter();

	ServiceMonthWidget* p;
	foreach(p, m_monthWidgets)
	{
		p->setFilter(m_filter);
		p->update();
	}
}

void ServiceCalendarWidget::createFilter()
{
	delete m_filter;

	AndFilter* andFilter = new AndFilter;

	// priest
	switch (m_priestOptions)
	{
	case PARTICULAR:
		if (m_priestId.isValid())
		{
            andFilter->add(new PriestFilter(m_priestId));
		}
		break;
	case ALL:
		// do nothing
		break;
	default:
		Q_ASSERT(0);
		break;
	}

	// church
	switch (m_churchOptions)
	{
	case PARTICULAR:
		if (m_churchId.isValid())
		{
            andFilter->add(new ChurchFilter(m_churchId));
		}
		break;
	case ALL:
		// do nothing
		break;
	default:
		Q_ASSERT(0);
		break;
	}

    m_filter = andFilter;
}

IServiceFilter* ServiceCalendarWidget::currentFilter()
{
	return m_filter;
}

void ServiceCalendarWidget::updateAllOccupied()
{
    const WaitingCursor sandHours;

	foreach (ServiceMonthWidget* w, m_monthWidgets)
	{
		w->updateOccupied();
	}
}

void ServiceCalendarWidget::refreshHolidayProvider()
{
    m_holidayProvider->refresh();
}

IHolidays *ServiceCalendarWidget::holidayProvider() const
{
    return m_holidayProvider;
}

