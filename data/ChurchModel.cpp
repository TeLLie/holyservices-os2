/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ChurchModel.h"

#include <QStringList>
#include <QPixmap>
#include <QtDebug>
#include <assert.h>

#include "ChurchIndexing.h"
#include "Church.h"

ChurchModel::ChurchModel(QObject *parent) : super(parent)
{
}

ChurchModel::~ChurchModel()
{
}

int ChurchModel::rowCount ( const QModelIndex& ) const
{
	return ChurchIndexing::allSet().size();
}

int ChurchModel::columnCount ( const QModelIndex& /* parent */) const
{
    return 3;
}

QVariant ChurchModel::data ( const QModelIndex& index, int role ) const
{
	if (index.isValid())
	{
		switch (role)
		{
		case Qt::DisplayRole:
		case Qt::EditRole:
			{
				const tChurchVector churchs = ChurchIndexing::allSet().data();

				if (index.row() < churchs.size())
				{
					const Church * const pChurch = churchs.at(index.row());
					
					switch (index.column())
					{
					case 0:	return QVariant(pChurch->place());
                    case 1:	return QVariant(pChurch->name());
                    case 2:	return QVariant(pChurch->abbreviation());
                    }
				}
			}
			break;
		case Qt::DecorationRole:
			{
				const tChurchVector churchs = ChurchIndexing::allSet().data();

				if (index.row() < churchs.size())
				{
					const Church * const pChurch = churchs.at(index.row());

					switch (index.column())
					{
					case 0:	
						{
							QPixmap dayColorDot(16, 16);
							dayColorDot.fill(pChurch->color());

							return dayColorDot;
						}
						break;
					}
				}
			}
			break;
		case Qt::UserRole:
			{
				const tChurchVector churches = ChurchIndexing::allSet().data();

				if (index.row() < churches.size())
				{
					const Church * const pChurch = churches.at(index.row());

					return QVariant(pChurch->getId().toString());
				}
			}
			break;
		};
	}
	return QVariant();
}

QVariant ChurchModel::headerData ( int section, Qt::Orientation orientation, 
		int role ) const
{
	if (section >= 0)
	{
		switch (role)
		{
		case Qt::DisplayRole:
			if (orientation == Qt::Horizontal)
			{
                static const QStringList headerDescriptions =
                        QStringList() << tr("Place") << tr("Church name") <<
                                         tr("Abbreviation");

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

Qt::ItemFlags ChurchModel::flags ( const QModelIndex& index ) const
{
	Qt::ItemFlags retVal = super::flags(index);
	
	retVal |= Qt::ItemIsEditable;

	return retVal;
}

bool ChurchModel::setData ( const QModelIndex& index, 
		const QVariant& value, int role )
{
	if (index.isValid() && index.internalPointer() && 
		(role == Qt::EditRole) && (value.canConvert(QVariant::String)))
	{
		Church *pChurch = static_cast<Church *>(index.internalPointer());
		const QString str = value.toString().trimmed();
		QModelIndex reallyChangedIndex(index);

		switch (index.column())
		{
		case 0: 
			{
				if (str != pChurch->place())
				{
					emit layoutAboutToBeChanged();

					pChurch->setPlace(str); 
					ChurchIndexing::allSet().sort();

					reallyChangedIndex = createIndex(ChurchIndexing::allSet().data().indexOf(pChurch),
						index.column(), pChurch);

					emit layoutChanged();
				}
			}
			break;
        case 1: pChurch->setName(str); break;
        case 2: pChurch->setAbbreviation(str); break;
        default: assert(0);
		}		
		
		emit dataChanged(reallyChangedIndex, reallyChangedIndex);

		return true;
	}
	return false;
}

QModelIndex ChurchModel::index(int row, int column, const QModelIndex& /* parent */) const
{
	tChurchVector churchs = ChurchIndexing::allSet().data();
	Church * pChurch = nullptr;

	if ((row >= 0) && (row < churchs.size()))
	{
		pChurch = churchs.at(row);
	}
	return createIndex(row, column, pChurch);
}

bool ChurchModel::insertRows ( int /* row */, int count, const QModelIndex& parent)
{
	assert(count == 1);

	if (1 == count)
	{
		const int rows = rowCount(parent);

		beginInsertRows(parent, rows, rows + count - 1);

		Church* pNewChurch = new Church;

		pNewChurch->setId(IdTag::createInitialized());
		ChurchIndexing::allSet().addElement(pNewChurch);

		endInsertRows();

		return true;
	}
	return false;
}

bool ChurchModel::removeRows ( int row, int count, const QModelIndex& parent)
{
	assert(count == 1);

	if (1 == count)
	{
		beginRemoveRows(parent, row, row + count - 1);

		{ // remove from memory structure
			Church *pToDelete = ChurchIndexing::allSet().data().at(row);

			ChurchIndexing::allSet().deleteElement(pToDelete);
		}
		
		endRemoveRows();
		return true;
	}
	return false;
}
