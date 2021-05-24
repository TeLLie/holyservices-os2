/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "AboutDlg.h"
#include "ui_AboutDlg.h"

#include "ButtonDecorator.h"

#include <QPushButton>

AboutDlg::AboutDlg(QWidget* parent) : super(parent)
{
    m_ui = new Ui_AboutDlg;
    m_ui->setupUi(this);

    m_ui->labelVersion->setText(tr("Version %1", "%1 stands for current version number").
                arg("1.0.0.38"));

    ButtonDecorator::assignIcons(*m_ui->buttonBox);
}

AboutDlg::~AboutDlg()
{
    delete m_ui;
}

