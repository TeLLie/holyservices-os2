#ifndef PRIESTMODEL_H_INCLUDED_0B0CBB94_883E_48CF_BED4_46A1EF8BB8F9
#define PRIESTMODEL_H_INCLUDED_0B0CBB94_883E_48CF_BED4_46A1EF8BB8F9

///\class PriestModel
///\brief Table model of priest allset
///\date 10-2006
///\author Jan 'Kovis' Struhar
#include <QAbstractTableModel>

class PriestModel : public QAbstractTableModel
{
	Q_OBJECT
	typedef QAbstractTableModel super;
public:
    PriestModel(QObject *parent);
    virtual ~PriestModel();

	virtual int rowCount ( const QModelIndex& parent ) const;
	virtual int columnCount ( const QModelIndex& parent ) const;
	virtual QVariant data ( const QModelIndex& index, int role ) const;
	virtual QVariant headerData ( int section, 
		Qt::Orientation orientation, int role ) const;
	
	virtual QModelIndex index(int row, int column, 
		const QModelIndex & parent) const;

	virtual Qt::ItemFlags flags ( const QModelIndex& index ) const;
	virtual bool setData ( const QModelIndex& index, 
		const QVariant& value, int role );

	virtual bool insertRows ( int row, int count, const QModelIndex& parent);
	virtual bool removeRows ( int row, int count, const QModelIndex& parent);
};

#endif
