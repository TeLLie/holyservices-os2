/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ButtonDecorator.h"

#include <QDialogButtonBox>
#include <QPushButton>

ButtonDecorator::ButtonDecorator()
{
}

ButtonDecorator::~ButtonDecorator()
{
}

void ButtonDecorator::assignIcons(QDialogButtonBox& box)
{
	QPushButton* btn;

	btn = box.button(QDialogButtonBox::Ok);
	if (btn)
	{
		btn->setIcon(QIcon(":images/button_ok.png"));
	}

	btn = box.button(QDialogButtonBox::Cancel);
	if (btn)
	{
		btn->setIcon(QIcon(":images/button_cancel.png"));
	}

        btn = box.button(QDialogButtonBox::Close);
        if (btn)
        {
                btn->setIcon(QIcon(":images/leave.png"));
        }

        btn = box.button(QDialogButtonBox::Save);
        if (btn)
        {
                btn->setIcon(QIcon(":images/filesave.png"));
        }
}

