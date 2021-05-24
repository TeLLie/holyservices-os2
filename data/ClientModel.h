#ifndef CLIENTMODEL_H_INCLUDED_A435963D_BFAF_4B50_9F38_8F81754C62EA
#define CLIENTMODEL_H_INCLUDED_A435963D_BFAF_4B50_9F38_8F81754C62EA

///\class ClientModel
/// Model for table representation of the client allset (all clients in indexed list)
///\date 10-2006
///\author Jan 'Kovis' Struhar
#include <QAbstractTableModel>

class ClientModel : public QAbstractTableModel
{
	Q_OBJECT
	typedef QAbstractTableModel super;

public:
    ClientModel(QObject *parent);

    void setReadOnly(bool val);

    virtual int rowCount ( const QModelIndex& parent ) const override;
    virtual int columnCount ( const QModelIndex& parent ) const override;
    virtual QVariant data ( const QModelIndex& index, int role ) const override;
	virtual QVariant headerData ( int section, 
        Qt::Orientation orientation, int role ) const override;
	
	virtual QModelIndex index(int row, int column, 
        const QModelIndex & parent) const override;

    virtual Qt::ItemFlags flags ( const QModelIndex& index ) const override;
	virtual bool setData ( const QModelIndex& index, 
        const QVariant& value, int role ) override;

    virtual bool insertRows ( int row, int count, const QModelIndex& parent) override;
    virtual bool removeRows ( int row, int count, const QModelIndex& parent) override;

private:
    enum Columns { SurnameColumn = 0, FirstNameColumn, StreetColumn,
                   ContactColumn, ColumnCount};
    bool m_readOnly = false;
};

#endif
