#ifndef PRIESTCOMBOBOX_H_INCLUDED_24EBA190_AEF9_42F4_A703_E6997C1C8DD0
#define PRIESTCOMBOBOX_H_INCLUDED_24EBA190_AEF9_42F4_A703_E6997C1C8DD0

///\class CPriestComboBox
/// Selection of a priest from priest allset
///\date 12-2009
///\author Jan 'Kovis' Struhar

#include "IdComboBox.h"

class CPriestComboBox : public IdComboBox
{
	typedef IdComboBox super;
public:
	CPriestComboBox(QWidget* p);
	virtual ~CPriestComboBox();

	virtual void fillCombo();
};

#endif

