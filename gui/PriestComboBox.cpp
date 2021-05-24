#include "PriestComboBox.h"

#include "PriestModel.h"

CPriestComboBox::CPriestComboBox(QWidget* p) : super(p)
{
	fillCombo();
}

CPriestComboBox::~CPriestComboBox()
{
}

void CPriestComboBox::fillCombo()
{
	QAbstractItemView* view = createViewForComboPopup(QSize(220, 120));
	
	setModel(new PriestModel(this));
	setView(view);
}
