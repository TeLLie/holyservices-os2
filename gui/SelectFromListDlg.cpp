/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "SelectFromListDlg.h"
#include "ui_SelectFromListDlg.h"

#include <QStringListModel>

#include "ButtonDecorator.h"

SelectFromListDlg::SelectFromListDlg(QWidget *parent, const QStringList& list) : super(parent)
{
    m_ui = new Ui_SelectFromListDlg;
    m_ui->setupUi(this);
    QStringListModel *model = new QStringListModel(list, this);
    m_ui->listView->setModel(model);
    m_ui->listView->selectionModel()->select(model->index(0), QItemSelectionModel::SelectCurrent);

    connect(m_ui->listView, SIGNAL(doubleClicked (const QModelIndex&)),
            SLOT(listDoubleClicked(const QModelIndex&)));

    ButtonDecorator::assignIcons(*m_ui->buttonBox);
}

SelectFromListDlg::~SelectFromListDlg()
{
    delete m_ui;
}

void SelectFromListDlg::listDoubleClicked ( const QModelIndex& )
{
    accept();
}

int SelectFromListDlg::currentItemIndex() const
{
    return m_ui->listView->currentIndex().row();
}
