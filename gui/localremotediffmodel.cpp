/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "localremotediffmodel.h"
#include "HolyService.h"
#include "../output/ReportColumnDescription.h"

#include <QColor>
#include <QBrush>

LocalRemoteDiffModel::LocalRemoteDiffModel(QObject *parent) : super(parent)
{
}

QVariant LocalRemoteDiffModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
    {
        switch (section)
        {
        case LOCAL_DATA: return tr("Local data");
        case REMOTE_DATA: return tr("Internet data");
        }
    }
    return QVariant();
}

int LocalRemoteDiffModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_diffData.size();
}

int LocalRemoteDiffModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;

    return COLUMN_COUNT;
}

QVariant LocalRemoteDiffModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    static const QBrush localNew(QColor(200, 255, 200));
    static const QBrush remoteNew(QColor(255, 200, 200));
    const LocalAndRemote& entry = m_diffData[index.row()];

    switch(role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
        case LOCAL_DATA:
            if (entry.m_local)
            {
                QString text = QString("%1 - %2...").arg(
                            ReportColumnDescription::columnData(ReportColumnDescription::ServiceStartDateShort, entry.m_local, nullptr),
                            entry.m_local->intention().left(40));
                if (entry.m_remote.isValid())
                {
                    text += entry.localToRemoteDiff("\n  ");
                }
                return text;
            }
            break;
        case REMOTE_DATA:
            if (entry.m_remote.isValid())
            {
                QString text = QString("%1 - %2...").arg(
                            entry.m_remote.m_start.toString("d. M."),
                            entry.m_remote.m_description.left(40));
                text += entry.remoteToLocalDiff("\n  ");
                return text;
            }
            break;
        }
        break;
    case Qt::BackgroundRole:
        if (entry.m_remote.isValid() && (entry.m_local == nullptr))
        {
            return (entry.m_remote.m_holyServiceGuid.isEmpty()) ? localNew : remoteNew;
        }
        if ((! entry.m_remote.isValid()) && entry.m_local) return localNew;
        break;
    }

    return QVariant();
}

void LocalRemoteDiffModel::setDiffData(const QVector<LocalAndRemote> &diffData)
{
    beginResetModel();
    m_diffData = diffData;
    endResetModel();
}
