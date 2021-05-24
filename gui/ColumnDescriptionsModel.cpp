/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ColumnDescriptionsModel.h"

#include "HolyServiceIndexing.h"

#include <QStringList>
#include <QMimeData>
#include <QTextStream>
#include <QtDebug>

// separator in text of mime data drag/drop
const QChar mimeSeparator(',');

ColumnDescriptionsModel::ColumnDescriptionsModel(QObject* p) :
    super(p), m_holidays(nullptr)
{
}

ColumnDescriptionsModel::~ColumnDescriptionsModel() {}

Qt::DropActions ColumnDescriptionsModel::supportedDragActions () const
{
	return Qt::MoveAction;
}

Qt::DropActions ColumnDescriptionsModel::supportedDropActions () const
{
	return Qt::MoveAction;
}

void ColumnDescriptionsModel::setColumnData(const tColumnDescriptions& cols)
{
    beginResetModel();
	m_data = cols;
    endResetModel();
}

const tColumnDescriptions& ColumnDescriptionsModel::getColumnData() const
{
    return m_data;
}

void ColumnDescriptionsModel::setHolidays(const IHolidays *holidayProvider)
{
    m_holidays = holidayProvider;
}

int ColumnDescriptionsModel::rowCount(const QModelIndex& /*parent*/) const
{
	return 2;
}

int ColumnDescriptionsModel::columnCount(const QModelIndex& /*parent*/) const
{
	return m_data.size();
}

QVariant ColumnDescriptionsModel::data(const QModelIndex& index, int role) const
{
	switch(role)
	{
	case Qt::DisplayRole:
		{
            const ReportColumnDescription& desc = m_data.at(index.column());

			switch(index.row())
			{
			case 0:
                return ReportColumnDescription::columnHeader(desc.columnType());
				break;
			case 1: // data preview
				{
					const HolyService * const hs = HolyServiceIndexing::allSet().find(m_previewedService);
					if (hs)
					{
                        return ReportColumnDescription::columnData(
                                    desc.columnType(), hs, m_holidays);
					}
					else
					{
						return tr("n/a in this version");
					}
				}
				break;
			}
		}
		break;
	case Qt::EditRole:
		// always the column type
		return m_data.at(index.column()).columnType();
    case Qt::TextAlignmentRole:
        switch (m_data.at(index.column()).columnType())
        {
        case ReportColumnDescription::Stipend:
            return Qt::AlignRight;
        default:
            return Qt::AlignLeft;
        }
	}
	return QVariant();
}

bool ColumnDescriptionsModel::setData(const QModelIndex& index, const QVariant& value, int /*role*/)
{
	if ((index.row() == 0) && value.canConvert<int>() && (index.column() < m_data.size()))
	{
        m_data[index.column()].setColumnType(static_cast<ReportColumnDescription::EType>(value.toInt()));
        emit dataChanged(index, createIndex(1, index.column(), nullptr));

		return true;
	}
	return false;
}

QVariant ColumnDescriptionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	switch(role)
	{
	case Qt::DisplayRole:
		if (orientation == Qt::Vertical)
		{
			switch (section)
			{
				case 0: return tr("Description");
				case 1: return tr("Preview");
			}
		}
		else
		{
			return QString("%L1").arg(section + 1);
		}
		break;
	}
	return QVariant();
}

bool ColumnDescriptionsModel::dropMimeData ( const QMimeData * data,
	Qt::DropAction action, int /*row*/, int /*column*/, const QModelIndex & parent )
{
    if (action == Qt::IgnoreAction)
	{
         return true;
	}

	if ((!data->hasText()) || data->text().isEmpty())
	{
        return false;
	}

	// convert the dropped values to list of integer column types
	qDebug() << "Dropped data " << data->text() << " on " << parent << " Action: " << action;
	QStringList droppedTextValues = data->text().split(mimeSeparator, QString::SkipEmptyParts);

	// remove the first record
	const QString dataSourceName = droppedTextValues.takeFirst();
	QList<int> droppedValues;

	foreach (const QString& aValue, droppedTextValues)
	{
		droppedValues << aValue.toInt();
	}
	
	if (! droppedValues.isEmpty())
	{
		// first from the dropped values is enough for decision,
		// because the all the dropped columns come from either hidden
		// or visible columns
		if (dataContainsColumnType(droppedValues.first())) // move within one model
		{
			foreach (int columnType, droppedValues)
			{
				const int originalColumn = columnOfColumnType(columnType);

				if (originalColumn >= 0)
				{
					removeColumn(originalColumn);
				}
			}

			// append on unknown parent
			const int targetColumn = (parent.column() >= 0) ? 
				parent.column() : columnCount(parent);

			// and insert on different place
			insertColumns(targetColumn, droppedValues.count());

			for (int i = 0; i < droppedValues.count(); ++i)
			{
				setData(index(0, targetColumn + i), droppedValues.at(i));
			}
			return true;
		}
		else // move from different model
		{
			emit requestForRemoval(dataSourceName, droppedValues);

			// append on unknown parent
			const int targetColumn = (parent.column() >= 0) ? 
				parent.column() : columnCount(parent);

			insertColumns(targetColumn, droppedValues.count());

			for (int i = 0; i < droppedValues.count(); ++i)
			{
				setData(index(0, targetColumn + i), droppedValues.at(i));
			}
			return true;
		}
	}
	return false;
}

bool ColumnDescriptionsModel::dataContainsColumnType(int columnType) const
{
	return (columnOfColumnType(columnType) >= 0);
}

int ColumnDescriptionsModel::columnOfColumnType(int columnType) const
{
	int index = -1;
    foreach (const ReportColumnDescription& colDesc, m_data)
	{
		++index;
		if (colDesc.columnType() == columnType)
		{
			return index;
		}
	}
	return -1;
}

Qt::ItemFlags ColumnDescriptionsModel::flags(const QModelIndex &index) const
{
     Qt::ItemFlags defaultFlags = super::flags(index);

     if (index.isValid())
	 {
         return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
	 }
     else
	 {
         return Qt::ItemIsDropEnabled | defaultFlags;
	 }
}

QStringList ColumnDescriptionsModel::mimeTypes() const
{
	return QStringList() << "text/plain";
}

QMimeData* ColumnDescriptionsModel::mimeData(const QModelIndexList &indexes) const
{
	// collect a unique set of values in the indexes - order is important
	QList<int> draggedValues;

	foreach (const QModelIndex& index, indexes) 
	{
		const int indexData = index.data(Qt::EditRole).toInt();

		if (! draggedValues.contains(indexData))
		{
			draggedValues << indexData;
		}
	}
	
	QString resultString;
	QTextStream stream(&resultString);

	stream << objectName() << mimeSeparator;
	foreach(int columnValue, draggedValues)
	{
		stream << columnValue << mimeSeparator;
	}

	QMimeData *mimeData = new QMimeData();
	mimeData->setText(resultString);

	return mimeData;
} 

bool ColumnDescriptionsModel::insertRows ( int /*row*/, int /*count*/,
	const QModelIndex& /*parent*/ /*= QModelIndex()*/ )
{
	return false;
}

bool ColumnDescriptionsModel::removeRows ( int /*row*/, int /*count*/,
	const QModelIndex& /*parent*/ /*= QModelIndex()*/ )
{
	return false;
}

bool ColumnDescriptionsModel::insertColumns ( int column, int count,
	const QModelIndex & parent)
{
	const bool bAppend = (column == columnCount());

	beginInsertColumns(parent, column, column + count - 1);

	for (int i = 0; i < count; ++i)
	{
		if (bAppend)
		{
            m_data.append(ReportColumnDescription());
		}
		else
		{
            m_data.insert(column, ReportColumnDescription());
		}
	}
	endInsertColumns();

	return true;
}

bool ColumnDescriptionsModel::removeColumns ( int column, int count,
	const QModelIndex & parent)
{
	beginRemoveColumns(parent, column, column + count - 1);

	for (int i = 0; i < count; ++i)
	{
		m_data.removeAt(column);
	}

	endRemoveColumns();

	return (count > 0);
}

void ColumnDescriptionsModel::removeColumnTypes(const QList<int>& columnTypes)
{
	foreach (const int columnType, columnTypes)
	{
		const int columnIndex = columnOfColumnType(columnType);

		Q_ASSERT(columnIndex >= 0);

		if (columnIndex >= 0)
		{
			removeColumn(columnIndex);
		}
	}
}

bool ColumnDescriptionsModel::moveColumns( const QList<int>& columnTypes, int offset )
{
	if (offset == 0)
	{
		return false;
	}

	foreach(int columnType, columnTypes)
	{
		const int columnIndex = columnOfColumnType(columnType);

		Q_ASSERT(columnIndex >= 0);

		if (columnIndex >= 0)
		{
			const int targetIndex = columnIndex + offset;
			const bool targetIndexOk = (targetIndex >= 0) && (targetIndex <= m_data.size());

			Q_ASSERT(targetIndexOk);

			if (targetIndexOk)
			{
				emit layoutAboutToBeChanged();

                const ReportColumnDescription movedDescription = m_data.takeAt(columnIndex);
				m_data.insert(targetIndex, movedDescription);

				emit layoutChanged();
			}
			else 
			{
				return false;
			}
		}
	}
	return true;
}

int ColumnDescriptionsModel::columnType( int columnIndex ) const
{
	int retVal = -1;

	if ((columnIndex >= 0) && (columnIndex < m_data.size()))
	{
		return m_data.at(columnIndex).columnType();
	}
	return retVal;
}

void ColumnDescriptionsModel::setHolyServiceForPreview(const IdTag& holyServiceId)
{
	m_previewedService = holyServiceId;
}
