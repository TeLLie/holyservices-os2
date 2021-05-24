#ifndef REPORTCOLUMNCONFIGURATIONDLG_H_INCLUDED_D25250FD_21AB_4F44_9D8B_ABD72AAF6E4E
#define REPORTCOLUMNCONFIGURATIONDLG_H_INCLUDED_D25250FD_21AB_4F44_9D8B_ABD72AAF6E4E

///\class ReportColumnConfigurationDlg
/// Set up the report columns that have to be present in the report
/// via drag-drop or button click
///\date 4-2008
///\author Jan 'Kovis' Struhar

#include "ui_ReportColumnConfigurationDlg.h"
#include "../output/ReportColumnDescription.h"
//#include "ColumnDescriptionsModel.h"
#include <QDialog>

class ColumnDescriptionsModel;
class IdTag;

class ReportColumnConfigurationDlg : public QDialog, private Ui::ReportColumnConfigurationDlg
{
	Q_OBJECT
	typedef QDialog super;
public:
	ReportColumnConfigurationDlg(QWidget*);
	virtual ~ReportColumnConfigurationDlg();

	void setColumnConfiguration(const tColumnDescriptions&);
	tColumnDescriptions columnConfiguration() const; 

    void setHolidays(const IHolidays* holidayProvider);

private slots:
	void hideColumn();

	void exposeColumn();
	/// move regards only the visible part
	void moveColumnLeft();
	void moveColumnRight();

	void removeColumnType(const QString& modelName, QList<int> columnTypes);

private:
    ColumnDescriptionsModel *m_visibleDataModel;
    ColumnDescriptionsModel *m_hiddenDataModel;
    const IHolidays* m_holidays;

	/// assume selection comes from a CColumnDescriptionModel
	QList<int> getSelectedTypes( const QModelIndexList& selection );

	/// on move left/right the selection does not move with the columns, reselect them
	void setTypesAsSelected(const QList<int>&);

	/// recently modified service
	IdTag mostRecentlyModifiedService() const;
};

#endif
