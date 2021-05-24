/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/


#ifndef HOLIDAYLISTMODEL_H
#define HOLIDAYLISTMODEL_H

#include <QAbstractTableModel>
#include "../holidays/HolidayRecord.h"

class HolidayListModel : public QAbstractTableModel
{
    typedef QAbstractTableModel super;
    Q_OBJECT
public:
    explicit HolidayListModel(QObject *parent = nullptr);
    virtual ~HolidayListModel();

    enum { DateColumn = 0, HolidayTextColumn, TotalColumns };

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool insertRows(int row, int count, const QModelIndex &parent);
    virtual bool removeRows(int column, int count, const QModelIndex &parent);

    bool storeData() const;

signals:
    void dirtyChanged(bool);

private:
    QList<HolidayRecord> m_list;
    bool m_dirty;
};

#endif // HOLIDAYLISTMODEL_H
