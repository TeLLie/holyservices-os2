/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "HolyServiceSelection.h"

#include "HolyService.h"
#include "ButtonDecorator.h"

#include <QMessageBox>
#include <QHeaderView>

CHolyServiceSelection::CHolyServiceSelection(QWidget *parent) : QDialog(parent)
{
	setupUi(this);

	m_selected = nullptr;

	serviceTable->verticalHeader()->setDefaultSectionSize(
		serviceTable->verticalHeader()->minimumSectionSize());

	ButtonDecorator::assignIcons(*buttonBox);

	connect(buttonBox, SIGNAL(accepted()), SLOT(onOkPressed()));
	connect(serviceTable, SIGNAL(cellDoubleClicked(int, int)), 
		SLOT(onCellDoubleClicked(int, int)));
}

CHolyServiceSelection::~CHolyServiceSelection()
{
}

HolyService * CHolyServiceSelection::getSelected(QWidget *parent, 
	const tHolyServiceVector& servicesForSelection)
{
	HolyService *retVal = nullptr;

	if (servicesForSelection.isEmpty())
	{
		QMessageBox::warning(parent, QObject::tr("Warning"),
			QObject::tr("There is no selection set provided."));
	}
	else
	{
		CHolyServiceSelection dlg(parent);

		dlg.setData(servicesForSelection);

		if (dlg.exec() == QDialog::Accepted)
		{
			retVal = dlg.getSelected();
		}
	}
		
	return retVal;
}

void CHolyServiceSelection::onOkPressed()
{
	const int nRow = serviceTable->currentRow();
	if (nRow >= 0)
	{
		m_selected = m_selectionBase.at(nRow);
	}
	accept();
}

void CHolyServiceSelection::onCellDoubleClicked(int, int)
{
	onOkPressed();
}

void CHolyServiceSelection::setData(const tHolyServiceVector& services)
{
	m_selectionBase = services;
	fillTable(services);
	serviceTable->setCurrentCell(0,0);
}

HolyService *CHolyServiceSelection::getSelected()
{
	return m_selected;
}

void CHolyServiceSelection::fillTable(const tHolyServiceVector& services)
{
	serviceTable->clearContents();
	serviceTable->setRowCount(services.size());

	QTableWidgetItem *pItem;
	const HolyService *pService;
	int currentRow = 0;
	const QFontMetrics fm(serviceTable->font());
	const tHolyServiceVectorConstIt itEnd = services.end();
	tHolyServiceVectorConstIt it = services.begin();

	for (; it != itEnd; ++it)
	{
		pService = *it;

		pItem = new QTableWidgetItem;
		const QString dateString = pService->startTime().toString(Qt::LocaleDate);
		pItem->setText(dateString);
		pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		serviceTable->setItem(currentRow, 0, pItem);

		pItem = new QTableWidgetItem;
		pItem->setText(pService->intention());
		pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		serviceTable->setItem(currentRow, 1, pItem);

		pItem = new QTableWidgetItem;
		pItem->setText(pService->clientText());
		pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		serviceTable->setItem(currentRow, 2, pItem);

		++currentRow;
	}
	serviceTable->resizeColumnsToContents();
}
