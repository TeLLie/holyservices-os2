/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ServiceMonthWidget.h"

#include "CalendarWidget.h"
#include "AssignedFilter.h"
#include "HolyServiceIndexing.h"
#include "WaitingCursor.h"
#include "HsTypedefs.h"
#include "ServiceDetailDlg.h"
#include "HolyServiceSelection.h"
#include "CurrentPersistentStore.h"
#include "IPersistentStore.h"
#include "HolyService.h"
#include "../preferences/UserSettings.h"
#include "AddSingleHolidayDlg.h"
#include "HolidayListModel.h"
#include "IHolidays.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QProgressBar>
#include <QMessageBox>
#include <Qt>
#include <QtDebug>
#include <QMenu>
#include <memory>

ServiceMonthWidget::ServiceMonthWidget(QWidget *parent) : super(parent)
{
	m_serviceFilter = nullptr;
    m_holidayProvider = nullptr;

    auto layout = new QVBoxLayout(this);

	layout->setSpacing(2);
	layout->setMargin(2);

	//setAutoFillBackground(true);
	//setBackgroundRole(QPalette::Light);

    auto labelLayout = new QHBoxLayout;

	m_monthLabel = new QLabel(this);
	m_monthLabel->setText(tr("<b>Month name</b>"));
	labelLayout->addWidget(m_monthLabel, 5);

	m_occupied = new QProgressBar(this);
	m_occupied->setFixedWidth(100);
	m_occupied->setFixedHeight(10);
	m_occupied->setRange(0, 100);
	m_occupied->setToolTip(tr("How many percents of holy services in this month is already assigned to a client?"));
	labelLayout->addWidget(m_occupied);

	layout->addLayout(labelLayout);

    m_monthCalendar = new CalendarWidget(this);
	m_monthCalendar->setFirstDayOfWeek(Qt::Monday);
	m_monthCalendar->setShowGrid(true);
	m_monthCalendar->setTodayMarked(false);
    m_monthCalendar->setHorizontalHeaderFormat(CalendarModel::ShortDayNames);
    m_monthCalendar->setVerticalHeaderFormat(CalendarModel::NoVerticalHeader);
	m_monthCalendar->setContextMenuPolicy(Qt::CustomContextMenu);
	
	layout->addWidget(m_monthCalendar);

    connect(m_monthCalendar, &QWidget::customContextMenuRequested,
        this, &ServiceMonthWidget::showContextMenu);
    connect(m_monthCalendar, &CalendarWidget::activated,
            this, &ServiceMonthWidget::editDate);

    {
        auto deleteAction = new QAction(this);
        deleteAction->setShortcut(QKeySequence(QKeySequence::Delete));
        deleteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
        connect(deleteAction, &QAction::triggered, this, &ServiceMonthWidget::handleDelete);
        addAction(deleteAction);
    }
}

void ServiceMonthWidget::setMonth(int month, int year)
{
	const QDate dtStart(year, month, 1);

	Q_ASSERT(dtStart.isValid());

	const QDate dtEnd(year, month, dtStart.daysInMonth());

	Q_ASSERT(dtEnd.isValid());

// until Czech locale is fixed
//	m_monthLabel->setText("<b>" + QLocale().monthName(month) + "</b>");
	m_monthLabel->setText("<b>" + monthName(month) + "</b>");
	
	m_monthCalendar->setDateRange(dtStart, dtEnd);
	updateOccupied();
}

void ServiceMonthWidget::setFilter(IServiceFilter *pNewFilter)
{
	m_serviceFilter = pNewFilter;
	m_monthCalendar->setFilter(pNewFilter);

	updateOccupied();
}

void ServiceMonthWidget::setHolidays(IHolidays* holidayProvider)
{
    m_holidayProvider = holidayProvider;
    m_monthCalendar->setHolidays(m_holidayProvider);
}

void ServiceMonthWidget::editDate(const QDate& date)
{
	bool newServiceCreated = false;
    std::unique_ptr<HolyService> toBeEdited;
	tHolyServiceVector forDate = HolyServiceIndexing::allSet().query(
		date, date, m_serviceFilter);

	switch (forDate.size())
	{
	case 0:
		qDebug() << "No holy service for activated date " << date;
		toBeEdited = createService(date);
		newServiceCreated = true;
		break;
	case 1:
        toBeEdited.reset(forDate.first());
		break;
	default:
		qDebug() << "Select among " << forDate.size() << " services.";
        toBeEdited.reset(CHolyServiceSelection::getSelected(this, forDate));
	}

	if (toBeEdited)
	{
		ServiceDetailDlg dlg(this);

        dlg.setService(toBeEdited.get());
		if (dlg.exec() == QDialog::Accepted)
		{
			// add newly created to global collection
			if (newServiceCreated)
			{
                HolyService* pConflict = nullptr;

                if (! HolyServiceIndexing::allSet().addElement(toBeEdited.get(), pConflict))
				{
					QMessageBox::warning(this, tr("Conflict"),
						tr("New service conflicts with existing one\n"
							"- new service will be discarded.\n\n"
							"Try to choose not conflicting date and time."));
                    toBeEdited.reset();
				}
			}
            saveServiceChanges();
            toBeEdited.release();
        }
        else
        {
            if (! newServiceCreated)
            {
                toBeEdited.release();
            }
        }
    }
}

void ServiceMonthWidget::updateOccupied()
{
	const AssignedFilter assignedFilter;
    const auto allInMonth = HolyServiceIndexing::allSet().query(
		m_monthCalendar->minimumDate(), m_monthCalendar->maximumDate(), 
		m_serviceFilter);
    const auto allCount = allInMonth.size();
    const auto assignedInMonthCount =
		HolyServiceIndexing::filter(allInMonth, &assignedFilter).size();

    auto percentageOfOccupied = 0;
	if (allCount > 0)
	{
		percentageOfOccupied = (assignedInMonthCount * 100) / allCount;
	}
	m_occupied->setValue(percentageOfOccupied);

    // refresh cell tables, assignment criteria might have changed, too
    m_monthCalendar->redrawCellBackgrounds();
}

void ServiceMonthWidget::showContextMenu(const QPoint& pt)
{
    const auto selectedDate = m_monthCalendar->selectedDate();
    auto servicesOnDate = HolyServiceIndexing::allSet().
		query(selectedDate, selectedDate, m_serviceFilter);

//	qDebug() << "Context menu " << selectedDate;
	
	QMenu popup(selectedDate.toString(), this);
    auto createAction = popup.addAction(QPixmap(":/images/add.png"),
        tr("Create holy service..."));

	createAction->setData(10);

	if (servicesOnDate.isEmpty())
	{
		popup.setDefaultAction(createAction);
	}
	else
	{
        auto editAction = popup.addAction(QPixmap(":/images/edit.png"),
                                              tr("Edit holy service..."));
		editAction->setData(20);
		popup.setDefaultAction(editAction);

		popup.addSeparator();
        popup.addAction(QPixmap(":/images/delete.png"), tr("Delete holy service..."),
                        this, SLOT(handleDelete()), QKeySequence(QKeySequence::Delete));
	}

    popup.addSeparator();
    {
        auto addHoliday = popup.addAction(QPixmap(":/images/kontact_date.png"),
                                              tr("Add holiday for this date..."));
        addHoliday->setData(40);
    }

    const auto selectedAction = popup.exec(m_monthCalendar->mapToGlobal(pt));
	if (selectedAction)
	{
		switch (selectedAction->data().toInt())
		{
		case 10:
			{
                auto newService = createService(selectedDate);
				ServiceDetailDlg dlg(this);

				dlg.setService(newService.get());
				if (dlg.exec() == QDialog::Accepted)
				{
                    HolyService* conflict = nullptr;
                    if (HolyServiceIndexing::allSet().addElement(newService.get(), conflict))
					{
                        saveServiceChanges();
                        newService.release(); // new service is now owned by allset
					}
					else
					{
						QMessageBox::warning(this, tr("Conflict"),
							tr("New service conflicts with existing one\n"
								"%1\n"
								"- new service will be discarded.\n\n"
								"Try to choose not conflicting date and time.").
								arg(conflict->toString()));
					}
				}
			}
			break;
		case 20:
			Q_ASSERT(servicesOnDate.size());
			editDate(selectedDate);
			break;
        case 40: // add holiday
            {
                AddSingleHolidayDlg dlg(this);
                dlg.setDate(selectedDate);
                if (dlg.exec() == QDialog::Accepted)
                {
                    addHoliday(selectedDate, dlg.description(), dlg.forNextYears());
                }
            }
            break;
		default:
            // delete handler goes this way, it is connecte ddirectly to the slot
            //Q_ASSERT(0);
			break;
		}
    }
}

void ServiceMonthWidget::handleDelete()
{
    const auto selectedDate = m_monthCalendar->selectedDate();
    auto servicesOnDate = HolyServiceIndexing::allSet().
        query(selectedDate, selectedDate, m_serviceFilter);

    HolyService* toBeDeleted = nullptr;
    if (servicesOnDate.size() > 1)
    {
        CHolyServiceSelection dlg(this);

        dlg.setData(servicesOnDate);
        dlg.setWindowTitle(tr("Which holy service should be deleted?"));
        if (dlg.exec() == QDialog::Accepted)
        {
            toBeDeleted = dlg.getSelected();
        }
    }
    else if (servicesOnDate.size() == 1)
    {
        toBeDeleted = servicesOnDate.first();
    }

    if (toBeDeleted)
    {
        const auto answer = QMessageBox::question(this, tr("Confirmation"),
            tr("Delete the holy service\n%1?").arg(toBeDeleted->toString()),
            QMessageBox::Yes | QMessageBox::No);
        if (answer == QMessageBox::Yes)
        {
            HolyServiceIndexing::allSet().deleteElement(toBeDeleted);
            CurrentPersistentStore::get()->deleteHolyService(toBeDeleted);
            delete toBeDeleted;
            toBeDeleted = nullptr;
        }
    }
}

std::unique_ptr<HolyService> ServiceMonthWidget::createService(const QDate& date)
{
    std::unique_ptr<HolyService> retVal(new HolyService);

	retVal->setId(IdTag::createInitialized());
    retVal->setChurch(UserSettings::get().defaultChurch());
    retVal->setPriest(UserSettings::get().defaultPriest());
	{
        const auto defaultStart = QDateTime(date, QTime(17, 0));
		
		retVal->setStartTime(defaultStart);
		retVal->setEndTime(defaultStart.addSecs(3600)); // 1 hour long
	}
	return retVal;
}

QString ServiceMonthWidget::monthName(int month) const
{
	if ((month >= 1) && (month <= 12))
	{
        static auto monthNames = QStringList() << tr("January") <<
			tr("February") << tr("March") << tr("April") << tr("May") <<
			tr("June") << tr("July") << tr("August") << tr("September") <<
			tr("October") << tr("November") << tr("December");

		return monthNames.at(month - 1);
	}
    return QString();
}

void ServiceMonthWidget::addHoliday(const QDate &date, const QString &text, bool forNextYears)
{
    if ((! date.isValid()) || text.isEmpty() )
    {
        return;
    }
    HolidayListModel holidays;

    const auto countOfNextYears  = (forNextYears) ? 10 : 0;
    for (auto yearIndex = 0; yearIndex <= countOfNextYears; ++yearIndex)
    {
        holidays.insertRow(0, QModelIndex());
        holidays.setData(holidays.index(0, HolidayListModel::DateColumn), date.addYears(yearIndex));
        holidays.setData(holidays.index(0, HolidayListModel::HolidayTextColumn), text);
    }
    holidays.storeData();

    if (m_holidayProvider)
    {
        m_holidayProvider->refresh();
    }
}

void ServiceMonthWidget::saveServiceChanges()
{
    const WaitingCursor hourGlass;
    auto allSet = HolyServiceIndexing::allSet().data();

    std::for_each(allSet.begin(), allSet.end(), 
        SaveModifiedService(CurrentPersistentStore::get()));

    updateOccupied();
}
