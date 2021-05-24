/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ReportColumnConfigurationDlg.h"

#include "ButtonDecorator.h"
#include "ColumnDescriptionsModel.h"
#include "HolyServiceIndexing.h"

#include <QtDebug>
#include <QMessageBox>
#include <QDateTime>

ReportColumnConfigurationDlg::ReportColumnConfigurationDlg(QWidget *p) : super(p)
{
	setupUi(this);

	m_visibleDataModel = nullptr;
	m_hiddenDataModel = nullptr;
    m_holidays = nullptr;

	ButtonDecorator::assignIcons(*buttonBox);

	connect( hideColumnButton, SIGNAL(clicked()), SLOT(hideColumn()));
	connect( exposeColumnButton, SIGNAL(clicked()), SLOT(exposeColumn()));
	connect( columnLeftButton, SIGNAL(clicked()), SLOT(moveColumnLeft()));
	connect( columnRightButton, SIGNAL(clicked()), SLOT(moveColumnRight()));
}

ReportColumnConfigurationDlg::~ReportColumnConfigurationDlg() {}

void ReportColumnConfigurationDlg::setColumnConfiguration(const tColumnDescriptions& columns)
{
	tColumnDescriptions visible, hidden;
	QSet<ReportColumnDescription::EType> inParameter;

	// go over parameters and sort the into proper subcontainers
	foreach (const ReportColumnDescription& desc, columns)
	{
		inParameter.insert(desc.columnType());

		if (desc.visible())
		{
			visible.append(desc);
		}
		else
		{
			hidden.append(desc);
		}
	}

	// columns that are not in parameters put into hidden columns
	for (int i = 0; i < ReportColumnDescription::ColumnCount; ++i)
	{
		const ReportColumnDescription::EType descType = static_cast<ReportColumnDescription::EType>(i);
		
		if (! inParameter.contains(descType))
		{
			ReportColumnDescription newDesc;

			newDesc.setColumnType(descType);
			newDesc.setVisible(false);

			hidden.append(newDesc);
		}
	}

	// find proper holyservice for the preview - the recently acquired for instance
	const IdTag serviceForPreview = mostRecentlyModifiedService();

	// and feed the models
	if (m_visibleDataModel == nullptr)
	{
		m_visibleDataModel = new ColumnDescriptionsModel(this);
		m_visibleDataModel->setHolyServiceForPreview(serviceForPreview);
        m_visibleDataModel->setHolidays(m_holidays);
		m_visibleDataModel->setObjectName("visible_columns");
		connect(m_visibleDataModel, 
			SIGNAL(requestForRemoval(const QString& , QList<int>)), 
			SLOT(removeColumnType(const QString&, QList<int>)));
		m_visibleColumns->setModel(m_visibleDataModel);
	}
	m_visibleDataModel->setColumnData(visible);

	if (m_hiddenDataModel == nullptr)
	{
		m_hiddenDataModel = new ColumnDescriptionsModel(this);
		m_hiddenDataModel->setHolyServiceForPreview(serviceForPreview);
        m_hiddenDataModel->setHolidays(m_holidays);
		m_hiddenDataModel->setObjectName("hidden_columns");
		connect(m_hiddenDataModel, 
			SIGNAL(requestForRemoval(const QString&, QList<int>)), 
			SLOT(removeColumnType(const QString&, QList<int>)));
		m_hiddenColumns->setModel(m_hiddenDataModel);
	}
	m_hiddenDataModel->setColumnData(hidden);
}

tColumnDescriptions ReportColumnConfigurationDlg::columnConfiguration() const
{
	if (m_visibleDataModel && m_hiddenDataModel)
	{
		// make sure that visible is visible and hidden hidden
		tColumnDescriptions retVal;
		foreach(ReportColumnDescription vis,  m_visibleDataModel->getColumnData())
		{
			vis.setVisible(true);
			retVal.append(vis);
		}
		foreach(ReportColumnDescription invis,  m_hiddenDataModel->getColumnData())
		{
			invis.setVisible(false);
			retVal.append(invis);
		}

		return retVal;
	}
    return tColumnDescriptions();
}

void ReportColumnConfigurationDlg::setHolidays(const IHolidays *holidayProvider)
{
    m_holidays = holidayProvider;
}

const int firstRow = 0;

void ReportColumnConfigurationDlg::hideColumn()
{
	const QModelIndexList selection = m_visibleColumns->selectionModel()->selectedColumns(firstRow); 

	if (selection.isEmpty())
	{
		return;
	}

	// move the selected into the hidden model
	const QList<int> selectedTypes = getSelectedTypes(selection);

	// insert them into hidden columns
	const int oldHiddenColumnCount = m_hiddenDataModel->columnCount();

	m_hiddenDataModel->insertColumns(oldHiddenColumnCount,
		selectedTypes.size());
	for (int i = 0; i < selectedTypes.size(); ++i)
	{
		m_hiddenDataModel->setData(m_hiddenDataModel->index(firstRow, oldHiddenColumnCount + i),
			selectedTypes.at(i));
	}

	// and remove from visible ones
	m_visibleDataModel->removeColumnTypes(selectedTypes);
}

void ReportColumnConfigurationDlg::exposeColumn()
{
	const QModelIndexList selection = m_hiddenColumns->selectionModel()->selectedColumns(firstRow); 

	if (selection.isEmpty())
	{
		return;
	}

	// move the selected into the hidden model
	const QList<int> selectedTypes = getSelectedTypes(selection);
	
	// insert them into visible columns
	const int oldVisibleColumnCount = m_visibleDataModel->columnCount();

	m_visibleDataModel->insertColumns(oldVisibleColumnCount,
		selectedTypes.size());
	for (int i = 0; i < selectedTypes.size(); ++i)
	{
		m_visibleDataModel->setData(m_visibleDataModel->index(firstRow, oldVisibleColumnCount + i),
			selectedTypes.at(i));
	}

	// and remove from visible ones
	m_hiddenDataModel->removeColumnTypes(selectedTypes);
}

void ReportColumnConfigurationDlg::moveColumnLeft()
{
	const QModelIndexList selection = m_visibleColumns->selectionModel()->selectedColumns(firstRow); 

	if (selection.isEmpty())
	{
		return;
	}
	
	// leftmost column cannot be put more left
	const QModelIndex columnThatCannotBeMoved = m_visibleDataModel->index(firstRow, 0);

	if (selection.contains(columnThatCannotBeMoved))
	{
		QMessageBox::information(this, tr("Information"), 
			tr("Sorry, leftmost column cannot be moved more left."));
		return;
	}
	const QList<int> selectedTypes = getSelectedTypes(selection);

	m_visibleDataModel->moveColumns(selectedTypes, -1);
	setTypesAsSelected(selectedTypes);
}

void ReportColumnConfigurationDlg::moveColumnRight()
{
	const QModelIndexList selection = m_visibleColumns->selectionModel()->selectedColumns(firstRow); 

	if (selection.isEmpty())
	{
		return;
	}

	// rightmost column cannot be put more left
	const QModelIndex columnThatCannotBeMoved = m_visibleDataModel->index(firstRow, m_visibleDataModel->columnCount() - 1);

	if (selection.contains(columnThatCannotBeMoved))
	{
		QMessageBox::information(this, tr("Information"), 
			tr("Sorry, rightmost column cannot be moved more right."));
		return;
	}
	
	const QList<int> selectedTypes = getSelectedTypes(selection);

	m_visibleDataModel->moveColumns(selectedTypes, +1);
	setTypesAsSelected(selectedTypes);
}

void ReportColumnConfigurationDlg::removeColumnType(const QString& modelName, QList<int> columnTypes)
{
	ColumnDescriptionsModel *pTargetModel = nullptr;

	if (modelName == m_visibleDataModel->objectName())
	{
		pTargetModel = m_visibleDataModel;
	}
	else if (modelName == m_hiddenDataModel->objectName())
	{
		pTargetModel = m_hiddenDataModel;
	}

	Q_ASSERT(pTargetModel);

	if (pTargetModel)
	{
		pTargetModel->removeColumnTypes(columnTypes);
	}
}

QList<int> ReportColumnConfigurationDlg::getSelectedTypes( const QModelIndexList& selection )
{
	QList<int> retVal;

	// find out selected column types
	foreach(const QModelIndex& index, selection)
	{
		retVal.append(index.data(Qt::EditRole).toInt());
	}
	return retVal;
}

void ReportColumnConfigurationDlg::setTypesAsSelected( const QList<int>& typesToBeSelected)
{
	QItemSelectionModel* selModel = m_visibleColumns->selectionModel();

	if (selModel)
	{
		selModel->clearSelection();

		for (int column = 0; column < m_visibleDataModel->columnCount(); ++column)
		{
			if (typesToBeSelected.contains(m_visibleDataModel->columnType(column)))
			{
				const QItemSelection columnSelection(
					m_visibleDataModel->index(0, column), m_visibleDataModel->index(1, column));
				selModel->select(columnSelection, QItemSelectionModel::Select);
			}
		}
	}
}

IdTag ReportColumnConfigurationDlg::mostRecentlyModifiedService() const
{
	IdTag retVal;
	QDateTime newestDate(QDate(1850, 1, 1)); // very old

	foreach(HolyService* hs, HolyServiceIndexing::allSet().data())
	{
		if (hs->lastModified() > newestDate)
		{
			newestDate = hs->lastModified();
			retVal = hs->getId();
		}
	}
	return retVal;
}
