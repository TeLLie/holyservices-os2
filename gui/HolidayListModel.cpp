/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "HolidayListModel.h"

#include "../holidays/HolidayComposition.h"
#include "../persistence/HolidayStorage.h"
#include "UserSettings.h"

#include <QApplication>
#include <QPalette>

HolidayListModel::HolidayListModel(QObject *parent) :
    super(parent)
{
    HolidayStorage::loadFromFile(
                HolidayComposition::userSpecificHolidaysFileName(
                    UserSettings::userConfigFolder()), m_list);
    m_dirty = false;
}

HolidayListModel::~HolidayListModel()
{
}

QVariant HolidayListModel::data(const QModelIndex &index, int role) const
{
    if ((! index.isValid()) || (index.column() >= TotalColumns) || (index.row() >= m_list.size()))
    {
        return QVariant();
    }

    const HolidayRecord& rec = m_list.at(index.row());

    switch(role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
        case DateColumn:
            return rec.date().toString(Qt::DefaultLocaleShortDate);
        case HolidayTextColumn:
            return rec.description();
        }
        break;
    case Qt::EditRole:
        switch (index.column())
        {
        case DateColumn:
            return rec.date();
        case HolidayTextColumn:
            return rec.description();
        }
        break;
    case Qt::BackgroundRole:
        if (rec.date().dayOfWeek() == Qt::Sunday)
        {
            return QApplication::palette().brush(QPalette::AlternateBase);
        }
        break;
    }

    return QVariant();
}

QVariant HolidayListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        if (orientation == Qt::Horizontal)
        {
            switch(section)
            {
            case DateColumn: return tr("Date");
            case HolidayTextColumn: return tr("Holiday description");
            }
        }
        break;
    }

    return super::headerData(section, orientation, role);
}

int HolidayListModel::rowCount(const QModelIndex &) const
{
    return m_list.size();
}

int HolidayListModel::columnCount(const QModelIndex &) const
{
    return TotalColumns;
}

bool HolidayListModel::setData(const QModelIndex& index, const QVariant &value, int role)
{
    if (! index.isValid())
    {
        return false;
    }

    HolidayRecord& rec = m_list[index.row()];
    bool newDataIsDifferent = false;

    switch (role)
    {
    case Qt::EditRole:
        switch (index.column())
        {
        case DateColumn:
            {
                const QDate newValue = value.toDate();
                if (newValue != rec.date())
                {
                    rec.setDate(newValue);
                    newDataIsDifferent = true;
                }
            }
            break;
        case HolidayTextColumn:
            {
                const QString newValue = value.toString();
                if (newValue != rec.description())
                {
                    rec.setDescription(newValue);
                    newDataIsDifferent = true;
                }
            }
            break;
        }
        break;
    }

    if (newDataIsDifferent)
    {
        if (! m_dirty)
        {
            emit dirtyChanged(true);
        }
        m_dirty = true;
        emit dataChanged(index, index);
    }
    return false;
}

Qt::ItemFlags HolidayListModel::flags(const QModelIndex &index) const
{
    return super::flags(index) | Qt::ItemIsEditable;
}

bool HolidayListModel::storeData() const
{
    return HolidayStorage::storeToFile(
                HolidayComposition::userSpecificHolidaysFileName(
                    UserSettings::userConfigFolder()), m_list);
}

bool HolidayListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if ((row < 0) || (row > m_list.size()) || (count < 0))
    {
        return false;
    }
    HolidayRecord newRecord;

    if (row == m_list.size())
    {
        if (! m_list.isEmpty())
        {
            newRecord.setDate(m_list.last().date().addDays(1));
        }
    }
    else
    {
        newRecord.setDate(m_list.at(row).date().addDays(1));
    }

    beginInsertRows(parent, row, row + count - 1);
    while(count--)
    {
        m_list.insert(row, newRecord);
    }
    endInsertRows();

    if (! m_dirty)
    {
        emit dirtyChanged(true);
    }
    m_dirty = true;
    return true;
}

bool HolidayListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count > 0)
    {
        beginRemoveRows(parent, row, row + count - 1);
        while (count--)
        {
            m_list.removeAt(row);
        }
        endRemoveRows();

        if (! m_dirty)
        {
            emit dirtyChanged(true);
        }
        m_dirty = true;

        return true;
    }
    return false;
}
