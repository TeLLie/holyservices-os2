#ifndef CLIENTCOMBOBOX_H
#define CLIENTCOMBOBOX_H

///\class ClientComboBox
/// Combo box offering selection of holy service clients -
/// invalid id string means - no client assignment, asterisk all clients
///\todo incremental completer - default one gets only the first letter
///\date 01-2009
///\author Jan 'Kovis' Struhar

#include "IdComboBox.h"

class ClientComboBox : public IdComboBox
{
    Q_OBJECT
    typedef IdComboBox super;
public:
    ClientComboBox(QWidget*);

    /// fills the combo box by client allset with the first and default item being -all clients-
    void fillCombo() override;
};

#endif // CLIENTCOMBOBOX_H
