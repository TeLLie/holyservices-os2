#ifndef HOLYSERVICESELECTION_H_INCLUDED_B03C49EC_FE2A_43DE_A7DA_99B2E981362E
#define HOLYSERVICESELECTION_H_INCLUDED_B03C49EC_FE2A_43DE_A7DA_99B2E981362E

#include "HsTypedefs.h"
#include "ui_HolyServiceSelection.h"
#include <QDialog>

///\class CHolyServiceSelection
///
///\date 10-2006
///\author Jan 'Kovis' Struhar

class CHolyServiceSelection : public QDialog, Ui_HolyServiceSelection
{
	Q_OBJECT
public:
	static HolyService * getSelected(QWidget *parent, 
		const tHolyServiceVector& servicesForSelection);

	CHolyServiceSelection(QWidget *parent);
	virtual ~CHolyServiceSelection();

	void setData(const tHolyServiceVector&);
	
	HolyService *getSelected();

private slots:
	void onOkPressed();
	void onCellDoubleClicked(int, int);

private:
	HolyService *m_selected;
	tHolyServiceVector m_selectionBase;

	void fillTable(const tHolyServiceVector&);
};

#endif
