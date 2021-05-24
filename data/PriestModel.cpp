/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "PriestModel.h"

#include <QStringList>
#include <QtDebug>

#include "PriestIndexing.h"
#include "Priest.h"

PriestModel::PriestModel(QObject *parent) : super(parent)
{
}

PriestModel::~PriestModel()
{
}

int PriestModel::rowCount ( const QModelIndex& ) const
{
	return PriestIndexing::allSet().size();
}

int PriestModel::columnCount ( const QModelIndex& ) const
{
	return 2;
}

QVariant PriestModel::data ( const QModelIndex& index, int role ) const
{
	if (index.isValid())
	{
		switch (role)
		{
		case Qt::DisplayRole:
		case Qt::EditRole:
			{
				const tPriestVector priests = PriestIndexing::allSet().data();

				if (index.row() < priests.size())
				{
					const Priest * const pPriest = priests.at(index.row());
					
					switch (index.column())
					{
					case 0:	return QVariant(pPriest->surname());
					case 1:	return QVariant(pPriest->firstName());
					}
				}
			}
			break;
		case Qt::UserRole:
			{
				const tPriestVector priests = PriestIndexing::allSet().data();

				if (index.row() < priests.size())
				{
					const Priest * const pPriest = priests.at(index.row());

					return QVariant(pPriest->getId().toString());
				}
			}
			break;
		}
	}
	return QVariant();
}

QVariant PriestModel::headerData ( int section, Qt::Orientation orientation, 
		int role ) const
{
	if (section >= 0)
	{
		switch (role)
		{
		case Qt::DisplayRole:
			if (orientation == Qt::Horizontal)
			{
				static const QStringList headerDescriptions = QStringList() << 
					tr("Surname") << tr("First name");

				if (section < headerDescriptions.size())
				{
					return headerDescriptions.at(section);
				}
			}
			break;
		}
	}
	
	return QVariant();
}

Qt::ItemFlags PriestModel::flags ( const QModelIndex& index ) const
{
	Qt::ItemFlags retVal = super::flags(index);
	
	retVal |= Qt::ItemIsEditable;

	return retVal;
}

bool PriestModel::setData ( const QModelIndex& index, 
		const QVariant& value, int role )
{
	if (index.isValid() && index.internalPointer() && 
		(role == Qt::EditRole) && (value.canConvert(QVariant::String)))
	{
		QModelIndex reallyChangedIndex(index);
		Priest *pPriest = static_cast<Priest *>(index.internalPointer());
		const QString str = value.toString().trimmed();

		switch (index.column())
		{
		case 0: 
			{
				if (str != pPriest->surname())
				{
					emit layoutAboutToBeChanged();

					pPriest->setSurname(str); 
					PriestIndexing::allSet().sort();
					
					reallyChangedIndex = createIndex(PriestIndexing::allSet().data().indexOf(pPriest),
						index.column(), pPriest);

					emit layoutChanged();
				}
			}
			break;
		case 1: pPriest->setFirstName(str); break;
		default: assert(0);
		}		
		
//		qDebug() << "Priest changed " << reallyChangedIndex;

		emit dataChanged(reallyChangedIndex, reallyChangedIndex);

		return true;
	}
	return false;
}

QModelIndex PriestModel::index(int row, int column, const QModelIndex&) const
{
	tPriestVector priests = PriestIndexing::allSet().data();
	Priest * pPriest = nullptr;

	if ((row >= 0) && (row < priests.size()))
	{
		pPriest = priests.at(row);
	}
	return createIndex(row, column, pPriest);
}

bool PriestModel::insertRows ( int /* row */, int count, const QModelIndex& parent)
{
	assert(count == 1);

	if (1 == count)
	{
		const int rows = rowCount(parent);

		beginInsertRows(parent, rows, rows + count - 1);

		Priest* pNewPriest = new Priest;

		pNewPriest->setId(IdTag::createInitialized());
		PriestIndexing::allSet().addElement(pNewPriest);

		endInsertRows();

		return true;
	}
	return false;
}

bool PriestModel::removeRows ( int row, int count, const QModelIndex& parent)
{
	assert(count == 1);

	if (1 == count)
	{
		beginRemoveRows(parent, row, row + count - 1);

		{ // remove from memory structure
			Priest *pToDelete = PriestIndexing::allSet().data().at(row);

			PriestIndexing::allSet().deleteElement(pToDelete);
		}
		
		endRemoveRows();
		return true;
	}
	return false;
}
