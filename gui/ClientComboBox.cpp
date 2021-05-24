/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ClientComboBox.h"
#include "ClientIndexing.h"

ClientComboBox::ClientComboBox(QWidget* p) : super(p)
{
    fillCombo();
}

void ClientComboBox::fillCombo()
{
	// special items first
    addItem(tr("<All clients>"), "*"); 
    addItem(tr("<not assigned>"), QString()); // empty ID

	// then clients sorted by default order
    foreach (Client* client, ClientIndexing::allSet().data())
    {
        addItem(client->toString(Client::SurnameNameStreet), client->getId().toString());
    }
}
