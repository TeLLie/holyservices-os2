/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ClientModel.h"

#include <QStringList>
#include <QtDebug>

#include "ClientIndexing.h"
#include "Client.h"

ClientModel::ClientModel(QObject *parent) : super(parent)
{
}

int ClientModel::rowCount ( const QModelIndex& ) const
{
	return ClientIndexing::allSet().size();
}

int ClientModel::columnCount ( const QModelIndex& ) const
{
    return ColumnCount;
}

QVariant ClientModel::data ( const QModelIndex& index, int role ) const
{
	if (index.isValid())
	{
		switch (role)
		{
		case Qt::DisplayRole:
		case Qt::EditRole:
			{
				const tClientVector clients = ClientIndexing::allSet().data();

				if (index.row() < clients.size())
				{
					const Client * const pClient = clients.at(index.row());
					
					switch (index.column())
					{
                                        case SurnameColumn:
                                            return QVariant(pClient->surname());
                                        case FirstNameColumn:
                                            return QVariant(pClient->firstName());
                                        case StreetColumn:
                                            return QVariant(pClient->street());
                                        case ContactColumn:
                                            return QVariant(pClient->contact());
                                        }
				}
			}
			break;
		case Qt::UserRole:
			{
				const tClientVector clients = ClientIndexing::allSet().data();

				if (index.row() < clients.size())
				{
					const Client * const pClient = clients.at(index.row());

					return QVariant(pClient->getId().toString());
				}
			}
			break;
		}
	}
	return QVariant();
}

QVariant ClientModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
	if (section >= 0)
	{
		switch (role)
		{
		case Qt::DisplayRole:
			if (orientation == Qt::Horizontal)
			{
				static const QStringList headerDescriptions = QStringList() << 
                                        tr("Surname") << tr("First name") << tr("Street") << tr("Contact");

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

Qt::ItemFlags ClientModel::flags ( const QModelIndex& index ) const
{
	Qt::ItemFlags retVal = super::flags(index);
	
	retVal |= Qt::ItemIsEditable;

	return retVal;
}

bool ClientModel::setData ( const QModelIndex& index, 
		const QVariant& value, int role )
{
    if (m_readOnly)
    {
        return false;
    }

	if (index.isValid() && index.internalPointer() && 
		(role == Qt::EditRole) && (value.canConvert(QVariant::String)))
	{
		QModelIndex reallyChangedIndex(index);
		Client *pClient = static_cast<Client *>(index.internalPointer());
		const QString str = value.toString().trimmed();

		switch (index.column())
		{
                case SurnameColumn:
			{
				if (str != pClient->surname())
				{
					emit layoutAboutToBeChanged();

					pClient->setSurname(str); 
					ClientIndexing::allSet().sort();

					reallyChangedIndex = createIndex(ClientIndexing::allSet().data().indexOf(pClient),
						index.column(), pClient);

					emit layoutChanged();
				}
			}
			break;
                case FirstNameColumn: pClient->setFirstName(str); break;
                case StreetColumn: pClient->setStreet(str); break;
                case ContactColumn: pClient->setContact(str); break;
                default: assert(0);
		}		
		
		emit dataChanged(reallyChangedIndex, reallyChangedIndex);

		return true;
	}
	return false;
}

QModelIndex ClientModel::index(int row, int column, const QModelIndex& ) const
{
	tClientVector clients = ClientIndexing::allSet().data();
	Client * pClient = nullptr;

	if ((row >= 0) && (row < clients.size()))
	{
		pClient = clients.at(row);
	}
	return createIndex(row, column, pClient);
}

bool ClientModel::insertRows ( int /* row */, int count, const QModelIndex& parent)
{
    if (m_readOnly)
    {
        return false;
    }

    assert(count == 1);

	if (1 == count)
	{
		const int rows = rowCount(parent);

		beginInsertRows(parent, rows, rows + count - 1);

		Client* pNewClient = new Client;

		pNewClient->setId(IdTag::createInitialized());
		ClientIndexing::allSet().addElement(pNewClient);

		endInsertRows();

		return true;
	}
	return false;
}

bool ClientModel::removeRows ( int row, int count, const QModelIndex& parent)
{
    if (m_readOnly)
    {
        return false;
    }

    assert(count == 1);

	if (1 == count)
	{
		beginRemoveRows(parent, row, row + count - 1);

		{ // remove from memory structure
			Client *pToDelete = ClientIndexing::allSet().data().at(row);

			ClientIndexing::allSet().deleteElement(pToDelete);
		}
		
		endRemoveRows();
		return true;
	}
	return false;
}

void ClientModel::setReadOnly( bool val )
{
    m_readOnly = val;
}
