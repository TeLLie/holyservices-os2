/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "IdComboBox.h"

#include "../common/IdTag.h"

#include <QTableView>
#include <QHeaderView>

IdComboBox::IdComboBox(QWidget* p) : super(p)
{
}

IdComboBox::~IdComboBox()
{
}

QAbstractItemView* IdComboBox::createViewForComboPopup( const QSize& minSize )
{
	QTableView* pView = new QTableView;

	pView->setMinimumSize(minSize);
	pView->setAlternatingRowColors(true);
	pView->setSelectionBehavior(QAbstractItemView::SelectRows);
	pView->horizontalHeader()->hide();
	pView->verticalHeader()->hide();

	return pView;
}

void IdComboBox::setCurrentId( const IdTag& id)
{
	QModelIndexList indices = model()->match(model()->index(0,0), Qt::UserRole, 
		id.toString(), 1, Qt::MatchExactly);

	if (indices.isEmpty())
	{
		setCurrentIndex(-1); // no current
	}
	else
	{
		setCurrentIndex(indices.first().row());
	}
}

QString IdComboBox::currentIdString() const
{
	if (currentIndex() >= 0)
	{
		return model()->index(currentIndex(), 0).data(Qt::UserRole).toString();
	}
	return QString();
}

IdTag IdComboBox::currentId() const
{
	const QString stringId = currentIdString();

	if (stringId.isEmpty())
	{
		return IdTag();
	}
	return IdTag::createFromString(stringId);
}

