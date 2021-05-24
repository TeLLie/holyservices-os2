/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "DateInputDlg.h"
#include "ui_DateInputDlg.h"

#include "ButtonDecorator.h"

DateInputDlg::DateInputDlg(QWidget* parent) : super(parent)
{
    m_ui = new Ui_DateInputDlg;
    m_ui->setupUi(this);
    ButtonDecorator::assignIcons(*m_ui->buttonBox);
}

DateInputDlg::~DateInputDlg()
{
    delete m_ui;
}

void DateInputDlg::setDate(const QDate& date)
{
    m_ui->dateEdit->setDate(date);
}

QDate DateInputDlg::getDate() const
{
    return m_ui->dateEdit->date();
}

