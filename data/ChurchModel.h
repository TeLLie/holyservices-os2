#ifndef CHURCHMODEL_H_INCLUDED_7193C89A_8AA6_40C1_8F9F_551551E0955A
#define CHURCHMODEL_H_INCLUDED_7193C89A_8AA6_40C1_8F9F_551551E0955A

///\class ChurchModel
///\brief Table model representation of church allset
///\date 10-2006
///\author Jan 'Kovis' Struhar
#include <QAbstractTableModel>

class ChurchModel : public QAbstractTableModel
{
	Q_OBJECT
	typedef QAbstractTableModel super;
public:
    ChurchModel(QObject *);
    virtual ~ChurchModel();

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
