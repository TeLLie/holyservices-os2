/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "UsualChurchCombo.h"

#include "ChurchIndexing.h"

UsualChurchCombo::UsualChurchCombo( QWidget* p ) : super(p)
{
}

UsualChurchCombo::~UsualChurchCombo()
{
}

void UsualChurchCombo::fillCombo()
{
    foreach (Church* pChurch, ChurchIndexing::allSet().data())
	{
		if (pChurch && pChurch->countOfActiveServices())
		{
			addItem(tr("%1 (%2 services a week)").arg(pChurch->toString()).arg(pChurch->countOfActiveServices()), 
				pChurch->getId().toString());
		}
	}
}

