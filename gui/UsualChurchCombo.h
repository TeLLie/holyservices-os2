#ifndef USUALCHURCHCOMBO_H_INCLUDED_4A7AB5DD_210A_4D6F_B3D2_44B25FB0EFD7
#define USUALCHURCHCOMBO_H_INCLUDED_4A7AB5DD_210A_4D6F_B3D2_44B25FB0EFD7

///\class UsualChurchCombo
///\brief Combo enumeration of slightly more detailed church info
///\date 2-2010
///\author Jan 'Kovis' Struhar 
#include "IdComboBox.h"

class UsualChurchCombo : public IdComboBox
{
    Q_OBJECT
    typedef IdComboBox super;
public:
    UsualChurchCombo(QWidget* p);
    virtual ~UsualChurchCombo();

    virtual void fillCombo();
};

#endif

