/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/


#ifndef HOLIDAYMANAGEMENTDLG_H
#define HOLIDAYMANAGEMENTDLG_H

#include <QDialog>

class QTableView;
class HolidayListModel;
class QAbstractButton;

class HolidayManagementDlg : public QDialog
{
    typedef QDialog super;
    Q_OBJECT
public:
    explicit HolidayManagementDlg(QWidget *parent = nullptr);

private slots:
    void addHoliday();
    void removeSelectedHoliday();
    void onSaveButton();
    void handleDirtyChanged(bool);

private:
    QTableView *m_table;
    HolidayListModel* m_model;
    QAbstractButton* m_saveButton;
};

#endif // HOLIDAYMANAGEMENTDLG_H
