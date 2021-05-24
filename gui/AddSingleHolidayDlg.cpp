/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "AddSingleHolidayDlg.h"
#include "ui_AddSingleHolidayDlg.h"

#include "ButtonDecorator.h"

AddSingleHolidayDlg::AddSingleHolidayDlg(QWidget *parent) :
    super(parent),
    m_ui(new Ui::AddSingleHolidayDlg)
{
    m_ui->setupUi(this);
    ButtonDecorator::assignIcons(*m_ui->buttonBox);
}

AddSingleHolidayDlg::~AddSingleHolidayDlg()
{
    delete m_ui;
}

void AddSingleHolidayDlg::setDate(const QDate& date)
{
    m_ui->dateLabel->setText(date.toString(Qt::SystemLocaleLongDate));
    m_date = date;
}

QString AddSingleHolidayDlg::description() const
{
    return m_ui->holidayEdit->text();
}

bool AddSingleHolidayDlg::forNextYears() const
{
    return m_ui->nextYearsBox->isChecked();
}
