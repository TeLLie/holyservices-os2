#ifndef COLUMNDESCRIPTIONSMODEL_H_INCLUDED_6F04A64D_6013_49FE_8CFB_483F9FCD34CB
#define COLUMNDESCRIPTIONSMODEL_H_INCLUDED_6F04A64D_6013_49FE_8CFB_483F9FCD34CB

///\class ColumnDescriptionsModel
/// Table model of columns that orders columns into model columns
/// and enables drag-drop of them
///\date 4-2008
///\author Jan 'Kovis' Struhar

#include "../output/ReportColumnDescription.h"
#include "../common/IdTag.h"

#include <QAbstractTableModel>

class ColumnDescriptionsModel : public QAbstractTableModel
{
	Q_OBJECT
	typedef QAbstractTableModel super;
public:
    ColumnDescriptionsModel(QObject*);
    virtual ~ColumnDescriptionsModel();

	void setColumnData(const tColumnDescriptions&);
	const tColumnDescriptions& getColumnData() const;

    void setHolidays(const IHolidays* holidayProvider);

    /// resolve the zero based column index into type the column refers to
	int columnType(int columnIndex) const;

    /// when the service is set, its contents is visible in the second row (preview)
	void setHolyServiceForPreview(const IdTag& holyServiceId);

    //@{
    ///\name Ancestor overrides
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	
	virtual bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
	virtual bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );	
	virtual bool insertColumns ( int column, int count, const QModelIndex& parent = QModelIndex() );	
	virtual bool removeColumns ( int column, int count, const QModelIndex& parent = QModelIndex() );
    //@}

	bool moveColumns(const QList<int>& columnTypes, int offset);

    //@{
    ///\name Drag&drop column support
	virtual Qt::DropActions supportedDragActions () const;
	virtual Qt::DropActions supportedDropActions () const;

	virtual QStringList mimeTypes() const;
	virtual QMimeData *mimeData(const QModelIndexList &indexes) const;

	virtual bool dropMimeData ( const QMimeData * data, Qt::DropAction action, 
		int row, int column, const QModelIndex & parent );
    //@}

	void removeColumnTypes(const QList<int>& columnTypes);

signals:
	void requestForRemoval(const QString& objectName, QList<int> columnTypes);

private:
	tColumnDescriptions m_data;
	IdTag m_previewedService;
    const IHolidays* m_holidays;

	bool dataContainsColumnType(int columnType) const;

	/// returns -1 if column type is not contained in model data
	int columnOfColumnType(int columnType) const;
};

#endif
