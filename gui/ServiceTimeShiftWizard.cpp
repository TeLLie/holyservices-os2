/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ServiceTimeShiftWizard.h"
#include "ui_ServiceTimeShiftWizard.h"

#include "HolyServiceIndexing.h"
#include "HolyService.h"
#include "daterangefilter.h"

#include <QtDebug>

ServiceTimeShiftWizard::ServiceTimeShiftWizard(QWidget *parent) :
    QWizard(parent),
    m_ui(new Ui::ServiceTimeShiftWizard)
{
    m_ui->setupUi(this);
}

ServiceTimeShiftWizard::~ServiceTimeShiftWizard()
{
    delete m_ui;
}

void ServiceTimeShiftWizard::on_yesterdayButton_clicked()
{
    m_ui->timeShiftMinutesBox->setValue(m_ui->timeShiftMinutesBox->value() - (60 * 24));
}

void ServiceTimeShiftWizard::on_oneHourEarlier_clicked()
{
    m_ui->timeShiftMinutesBox->setValue(m_ui->timeShiftMinutesBox->value() - (60 * 1));
}

void ServiceTimeShiftWizard::on_oneHourLater_clicked()
{
    m_ui->timeShiftMinutesBox->setValue(m_ui->timeShiftMinutesBox->value() + (60 * 1));
}

void ServiceTimeShiftWizard::on_tomorrowBtn_clicked()
{
    m_ui->timeShiftMinutesBox->setValue(m_ui->timeShiftMinutesBox->value() + (60 * 24));
}

void ServiceTimeShiftWizard::on_ServiceTimeShiftWizard_currentIdChanged(int id)
{
    switch (id)
    {
    case SELECTION_PAGE:
        if (m_previousId == 0)
        {
            DateRangeFilter dateRangeFilter;
            dateRangeFilter.setRange(m_ui->dateFromEdit->date(), m_ui->dateToEdit->date());
            m_dataForSelection = HolyServiceIndexing::filter(m_data, &dateRangeFilter);
            m_weekModel->setData(m_dataForSelection);
            m_ui->serviceTable->resizeColumnsToContents();
        }
        break;
    case SHIFT_PAGE:
        // fill the table according to selected indices in SELECTION_PAGE
        if (m_shiftedModel == nullptr)
        {
            m_shiftedModel = new ShiftedServicesTableModel(this);
            m_ui->shiftedTable->setModel(m_shiftedModel);
            m_ui->shiftedTable->setColumnWidth(ShiftedServicesTableModel::Intention, 250);
        }
        {
            const auto allSelected = m_ui->serviceTable->selectionModel()->selectedIndexes();
            QStringList selectedGuids;
            for (const auto& index : allSelected)
            {
                selectedGuids.append(index.data(Qt::EditRole).toStringList());
            }
            tHolyServiceVector toBeShifted;
            foreach(const QString& serviceGuid, selectedGuids.toSet())
            {
                toBeShifted << HolyServiceIndexing::allSet().find(IdTag::createFromString(serviceGuid));
            }
            toBeShifted.removeAll(nullptr);
            std::sort(toBeShifted.begin(), toBeShifted.end(), DateTimeAscendingSorting());
            m_shiftedModel->setData(toBeShifted);
            m_ui->shiftedTable->resizeColumnToContents(ShiftedServicesTableModel::OriginalTime);
            m_ui->shiftedTable->resizeColumnToContents(ShiftedServicesTableModel::ShiftedTime);
        }
        break;
    }
    m_previousId = id;
}

tHolyServiceVector ServiceTimeShiftWizard::servicesToBeShifted() const
{
    return m_shiftedModel->toBeShiftedData();
}

std::chrono::minutes ServiceTimeShiftWizard::wantedTimeShift() const
{
    return std::chrono::minutes(m_ui->timeShiftMinutesBox->value());
}

void ServiceTimeShiftWizard::setData(const tHolyServiceVector &sourceData)
{
    m_data = sourceData;
    m_ui->dateFromEdit->setDate(sourceData.first()->startTime().date());
    m_ui->dateToEdit->setDate(sourceData.last()->endTime().date());

    if (m_weekModel == nullptr)
    {
        m_weekModel = new WeekModel(this);
        m_ui->serviceTable->setModel(m_weekModel);
    }
}

void ServiceTimeShiftWizard::on_timeShiftMinutesBox_valueChanged(int currentTimeShift)
{
    m_shiftedModel->setTimeShift(std::chrono::minutes(currentTimeShift));
    m_ui->shiftedTable->resizeColumnToContents(ShiftedServicesTableModel::OriginalTime);
    m_ui->shiftedTable->resizeColumnToContents(ShiftedServicesTableModel::ShiftedTime);
}

//---------------------------------------------------------------------------

WeekModel::WeekModel(QObject *p) : super(p)
{
}

void WeekModel::setData(const tHolyServiceVector &sourceData)
{
    beginResetModel();
    m_data = sourceData;
    m_weekStart = m_weekEnd = -1;
    if (! m_data.isEmpty())
    {
        m_weekStart = m_data.first()->startTime().date().weekNumber();
        m_weekEnd = m_data.last()->startTime().date().weekNumber();
    }
    endResetModel();
}

int WeekModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    if (m_data.isEmpty()) return 0;

    // assumed data are from one year

    return m_weekEnd - m_weekStart + 1;
}

int WeekModel::columnCount(const QModelIndex&) const
{
    return ColumnCount;
}


QVariant WeekModel::data(const QModelIndex &index, int role) const
{
    if (! index.isValid()) return QVariant();

    if (role == Qt::TextAlignmentRole) return Qt::AlignRight;

    if (role == Qt::DisplayRole)
    {
        QString retVal;
        foreach(const HolyService* hs, servicesOnIndex(index))
        {
            if (! retVal.isEmpty()) retVal += "\n";
            retVal += hs->startTime().toString(QStringLiteral("d. M. h:mm"));
        }
        return retVal;
    }
    if (role == Qt::EditRole)
    {
        QStringList serviceGuids;
        foreach(const HolyService* hs, servicesOnIndex(index))
        {
            serviceGuids << hs->getId().toString();
        }
        return serviceGuids;
    }

    return QVariant();
}

QVariant WeekModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();

    switch (orientation)
    {
    case Qt::Vertical:
        if (role == Qt::DisplayRole)
        {
            return tr("Week %1").arg(m_weekStart + section);
        }
        break;
    case Qt::Horizontal:
        switch(section)
        {
        case 0: return tr("Mon");
        case 1: return tr("Tue");
        case 2: return tr("Wed");
        case 3: return tr("Thu");
        case 4: return tr("Fri");
        case 5: return tr("Sat");
        case 6: return tr("Sun");
        }

        break;
    }

    return QVariant();
}

tConstHolyServiceVector WeekModel::servicesOnIndex(const QModelIndex &index) const
{
    tConstHolyServiceVector retVal;
    if (! index.isValid()) return retVal;

    const int soughtWeekNr = m_weekStart + index.row();
    for (const HolyService* hs : m_data)
    { // traverse data until you find the right week and day in the week, rely on ordering
        const int currentWeekNr = hs->startTime().date().weekNumber();
        if (currentWeekNr < soughtWeekNr) continue;
        if (currentWeekNr > soughtWeekNr) return retVal;
        const int currentDayOfWeek = hs->startTime().date().dayOfWeek();
        if ((index.column() + 1) == currentDayOfWeek)
        {
            retVal << hs;
        }
    }
    return retVal;
}

//-----------------------------------------------------------------------------

ShiftedServicesTableModel::ShiftedServicesTableModel(QObject *p) : super(p) {}

void ShiftedServicesTableModel::setData(const tHolyServiceVector &sourceData)
{
    beginResetModel();
    m_data = sourceData;
    endResetModel();
}

void ShiftedServicesTableModel::setTimeShift(const std::chrono::minutes &timeShift)
{
    m_timeShift = timeShift;

    QModelIndex startChanged = index(0, ShiftedTime);
    QModelIndex endChanged = index(rowCount(), ShiftedTime);

    emit dataChanged(startChanged, endChanged);
}

tHolyServiceVector ShiftedServicesTableModel::toBeShiftedData() const
{
    return m_data;
}

int ShiftedServicesTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_data.size();
}

int ShiftedServicesTableModel::columnCount(const QModelIndex&) const
{
    return ColumnCount;
}

QVariant ShiftedServicesTableModel::data(const QModelIndex &index, int role) const
{
    if (! index.isValid()) return QVariant();
    const HolyService* const service = m_data.at(index.row());

    if (role == Qt::DisplayRole)
    {
        static const QString dtMask("ddd d.M h:mm");
        switch (index.column())
        {
        case Intention: return service->intention();
        case OriginalTime: return service->startTime().toString(dtMask);
        case ShiftedTime:
            {
                std::chrono::seconds shift = m_timeShift;
                return service->startTime().addSecs(shift.count()).toString(dtMask);
            }
        }
    }
    if (role == Qt::TextAlignmentRole)
    {
        switch(index.column())
        {
        case Intention: return Qt::AlignLeft;
        default: return Qt::AlignHCenter;
        }
    }
    return QVariant();
}

QVariant ShiftedServicesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();

    if (orientation == Qt::Vertical)
    {
        return QString::number(section + 1);
    }
    switch(section)
    {
    case Intention: return tr("Intention");
    case OriginalTime: return tr("Original time");
    case ShiftedTime: return tr("Shifted time");
    }

    return QVariant();
}
