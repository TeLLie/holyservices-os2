/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "HolidayManagementDlg.h"

#include "HolidayListModel.h"
#include "ButtonDecorator.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QTableView>
#include <QPushButton>
#include <QHeaderView>

HolidayManagementDlg::HolidayManagementDlg(QWidget *parent) : super(parent)
{
    QBoxLayout* mainLayout = new QVBoxLayout(this);
    m_table = new QTableView(this);
    m_model = new HolidayListModel(this);
    connect(m_model, SIGNAL(dirtyChanged(bool)), SLOT(handleDirtyChanged(bool)));

    mainLayout->addWidget(m_table);
    m_table->setModel(m_model);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->hide();
    m_table->setEditTriggers(QAbstractItemView::AllEditTriggers);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    m_saveButton = buttonBox->button(QDialogButtonBox::Save);
    {
        QAbstractButton* addBtn =
            buttonBox->addButton(tr("Add holiday"), QDialogButtonBox::ActionRole);
        connect(addBtn, SIGNAL(clicked()), SLOT(addHoliday()));
        addBtn->setIcon(QPixmap(":/images/add.png"));
    }
    {
        QAbstractButton* delBtn =
            buttonBox->addButton(tr("Remove selected holiday"), QDialogButtonBox::ActionRole);
        mainLayout->addWidget(buttonBox);
        connect(delBtn, SIGNAL(clicked()), SLOT(removeSelectedHoliday()));
        delBtn->setIcon(QPixmap(":/images/delete.png"));
    }
    ButtonDecorator::assignIcons(*buttonBox);

    connect(buttonBox, SIGNAL(accepted()), SLOT(onSaveButton()));
    connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));

    handleDirtyChanged(false);

    resize(570, 480);
}

void HolidayManagementDlg::addHoliday()
{
    const QModelIndex curr = m_table->currentIndex();
    int insertSpot = m_model->rowCount();

    if (curr.isValid())
    {
        insertSpot = curr.row();
    }
    m_model->insertRow(insertSpot);
}

void HolidayManagementDlg::removeSelectedHoliday()
{
    const QModelIndex curr = m_table->currentIndex();

    if (curr.isValid())
    {
        m_model->removeRow(curr.row());
    }
}

void HolidayManagementDlg::onSaveButton()
{
    m_model->storeData();
    accept();
}

void HolidayManagementDlg::handleDirtyChanged(bool isDirty)
{
    setWindowTitle((isDirty) ? tr("Holidays (modified)") : tr("Holidays"));
    m_saveButton->setEnabled(isDirty);
}
