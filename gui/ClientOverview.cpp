/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ClientOverview.h"
#include "ui_ClientOverview.h"

#include "ButtonDecorator.h"
#include "CurrentPersistentStore.h"
#include "IPersistentStore.h"
#include "ClientModel.h"
#include "PriestModel.h"
#include "ChurchModel.h"

#include "ClientFilter.h"
#include "PriestFilter.h"
#include "ChurchFilter.h"

#include "HolyServiceIndexing.h"

#include "Client.h"
#include "Priest.h"
#include "Church.h"

#include <QApplication>
#include <QAbstractButton>
#include <QPushButton>
#include <QMessageBox>
#include <QHeaderView>
#include <QtDebug>

const int ClientOverview::keyColumn = 0;

ClientOverview::ClientOverview(QWidget *parent, const EEditedObject editedType) : 
	QDialog(parent), m_editedType(editedType)
{
    m_ui = new Ui_ClientOverview;
    m_ui->setupUi(this);

	initializeWindowTitle();

	m_model = nullptr;

	setupButtons(); // initializes model also

    m_ui->clientView->setModel(m_model);

	initializeTableColumns();
	
	// enable queued connection
	qRegisterMetaType<QModelIndex>();

	// you have to wait for events to process before you move the edit focus
    connect(m_ui->clientView->itemDelegate(),
		SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)),
		SLOT(handleMoveOnKeyChange(QWidget*, QAbstractItemDelegate::EndEditHint)));

	connect(m_model, 
		SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		SLOT(noticeKeyIndexChange(const QModelIndex&)));
}

ClientOverview::~ClientOverview()
{
    delete m_ui;
}

void ClientOverview::handleMoveOnKeyChange(QWidget*, QAbstractItemDelegate::EndEditHint)
{
	if (m_lastChangedKeyIndex.isValid())
	{
		const QModelIndex nextIndex = 
			m_model->index(m_lastChangedKeyIndex.row(), m_lastChangedKeyIndex.column() + 1);

        m_ui->clientView->setCurrentIndex(nextIndex);
        m_ui->clientView->edit(nextIndex);

		m_lastChangedKeyIndex = QModelIndex();
	}
}

void ClientOverview::noticeKeyIndexChange(const QModelIndex& index)
{
	if (index.column() == keyColumn)
	{
		m_lastChangedKeyIndex = index;
	}
}

void ClientOverview::onDeleteButton()
{
    const QItemSelectionModel* selectionModel = m_ui->clientView->selectionModel();

	if (selectionModel)
	{
		QModelIndexList selectedRows = selectionModel->selectedIndexes();

		if (! selectedRows.isEmpty())
		{
			const QModelIndex selectedItemIndex = selectedRows.takeFirst();

			if (selectedItemIndex.isValid())
			{
				switch (m_editedType)
				{
				case CLIENT:
					deleteClient(selectedItemIndex);
					break;
				case CHURCH:
					deleteChurch(selectedItemIndex);
					break;
				case PRIEST:
					deletePriest(selectedItemIndex);
					break;
				case UNKNOWN:
					Q_ASSERT(0);
					break;
				}
			}
		}
	}
}

void ClientOverview::onAddButton()
{
	m_model->insertRows(0, 1, QModelIndex());

	QModelIndex newItemIndex = m_model->index(0, keyColumn);

    m_ui->clientView->setCurrentIndex(newItemIndex);
    m_ui->clientView->edit(newItemIndex);
}

void ClientOverview::setupButtons()
{
	QString addBtnText, deleteBtnText;

	switch (m_editedType)
	{
	case PRIEST:
        m_model = new PriestModel(this);
		addBtnText = tr("Add priest");
		deleteBtnText = tr("Delete priest ...");
		break;
	case CHURCH:
        m_model = new ChurchModel(this);
		addBtnText = tr("Add church");
		deleteBtnText = tr("Delete church ...");
		break;
	case CLIENT:
		m_model = new ClientModel(this);
		addBtnText = tr("Add client");
		deleteBtnText = tr("Delete client ...");
		break;
	case UNKNOWN:
		qFatal(qPrintable(tr("Unknown object type.")));
		break;
	}

    m_addButton = m_ui->buttonBox->addButton(addBtnText, QDialogButtonBox::ActionRole);
    m_deleteButton = m_ui->buttonBox->addButton(deleteBtnText, QDialogButtonBox::ActionRole);

	m_addButton->setIcon(QIcon(":images/add.png"));
	m_deleteButton->setIcon(QIcon(":images/delete.png"));
    ButtonDecorator::assignIcons(*m_ui->buttonBox);

	connect(m_addButton, SIGNAL(clicked()), SLOT(onAddButton()));
	connect(m_deleteButton, SIGNAL(clicked()), SLOT(onDeleteButton()));
    connect(m_ui->buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()),
		SLOT(reject()));
}

void ClientOverview::deleteClient(const QModelIndex& index)
{
	Client *pSelectedClient = 
		static_cast<Client *>(index.internalPointer());
	const QString additionalMsg = dependentMsg(pSelectedClient);

	const int answer = QMessageBox::question(this, tr("Confirmation"),
		tr("Really do you want to delete record for\n%1?\n\n%2").
		arg(pSelectedClient->toString(), additionalMsg), 
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

	if (answer == QMessageBox::Yes)
	{
		// remove from database
		if (CurrentPersistentStore::get()->deleteClient(pSelectedClient))
		{
			HolyServiceIndexing::allSet().removeClient(pSelectedClient);

			m_model->removeRow(index.row()); // remove from memory indexing

			// and remove from memory
			delete pSelectedClient;
			pSelectedClient = nullptr;
		}
	}
}

void ClientOverview::deletePriest(const QModelIndex& index)
{
	Priest *pSelectedPriest = 
		static_cast<Priest *>(index.internalPointer());
	const QString additionalMsg = dependentMsg(pSelectedPriest);

	const int answer = QMessageBox::question(this, tr("Confirmation"),
		tr("Really do you want to delete record for\n%1?\n\n%2").
		arg(pSelectedPriest->toString(), additionalMsg), 
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

	if (answer == QMessageBox::Yes)
	{
		// remove from database
		if (CurrentPersistentStore::get()->deletePriest(pSelectedPriest))
		{
			HolyServiceIndexing::allSet().removePriest(pSelectedPriest);

			m_model->removeRow(index.row()); // remove from memory indexing

			// and remove from memory
			delete pSelectedPriest;
			pSelectedPriest = nullptr;
		}
	}
}

void ClientOverview::deleteChurch(const QModelIndex& index)
{
	Church *pSelectedChurch = 
		static_cast<Church *>(index.internalPointer());
	const QString additionalMsg = dependentMsg(pSelectedChurch);

	const int answer = QMessageBox::question(this, tr("Confirmation"),
		tr("Really do you want to delete record for\n%1?\n\n%2").
		arg(pSelectedChurch->toString(), additionalMsg), 
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

	if (answer == QMessageBox::Yes)
	{
		// remove from database
		if (CurrentPersistentStore::get()->deleteChurch(pSelectedChurch))
		{
			HolyServiceIndexing::allSet().removeChurch(pSelectedChurch);

			m_model->removeRow(index.row()); // remove from memory indexing

			// and remove from memory
			delete pSelectedChurch;
			pSelectedChurch = nullptr;
		}
	}
}

QString ClientOverview::dependentMsg(Client * pClient)
{
	QString retVal;

	if (pClient)
	{
		const ClientFilter clientFilter(pClient->getId());
		const int nCount = HolyServiceIndexing::filter(
			HolyServiceIndexing::allSet().data(), &clientFilter).
			size();

		if (nCount > 0)
		{
			retVal = tr("There is %L1 holy services assigned to this client.\n"
				"If you delete the client, the assignment is canceled.").arg(nCount);
		}
	}
	return retVal;
}

QString ClientOverview::dependentMsg(Priest *pPriest)
{
	QString retVal;

	if (pPriest)
	{
		const PriestFilter priestFilter(pPriest->getId());
		const int nCount = HolyServiceIndexing::filter(
			HolyServiceIndexing::allSet().data(), &priestFilter).
			size();

		if (nCount > 0)
		{
			retVal = tr("There is %L1 holy services assigned to this priest.\n"
				"If you delete the priest, the assignment is canceled.").arg(nCount);
		}
	}
	return retVal;
}

QString ClientOverview::dependentMsg(Church *pChurch)
{
	QString retVal;

	if (pChurch)
	{
        const ChurchFilter churchFilter(pChurch->getId());
		const int nCount = HolyServiceIndexing::filter(
			HolyServiceIndexing::allSet().data(), &churchFilter).
			size();

		if (nCount > 0)
		{
			retVal = tr("There is %L1 holy services assigned to this church.\n"
				"If you delete the church, the assignment is canceled.").arg(nCount);
		}
	}
	return retVal;
}

void ClientOverview::initializeTableColumns()
{
	// set column defaults
    m_ui->clientView->verticalHeader()->hide();
    m_ui->clientView->verticalHeader()->setDefaultSectionSize(
        m_ui->clientView->verticalHeader()->minimumSectionSize());

    m_ui->clientView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_ui->clientView->horizontalHeader()->setStretchLastSection(true);

	switch(m_editedType)
	{
	case PRIEST:
        m_ui->clientView->setColumnWidth(0, width() / 2);
		break;
	case CHURCH:
        m_ui->clientView->setColumnWidth(0, width() / 3);
        m_ui->clientView->setColumnWidth(1, width() / 2);
        break;
	case CLIENT:
        m_ui->clientView->setColumnWidth(0, width() / 4);
        m_ui->clientView->setColumnWidth(1, width() / 4);
		break;
	case UNKNOWN:
	default:
		Q_ASSERT(0);
	}
}

void ClientOverview::initializeWindowTitle()
{
	QString title;

	switch(m_editedType)
	{
	case PRIEST:
		title = tr("Priest overview");
		break;
	case CHURCH:
		title = tr("Church overview");
		break;
	case CLIENT:
		title = tr("Client overview");
		break;
	default:
		Q_ASSERT(0);
	}
	setWindowTitle(title);
}
