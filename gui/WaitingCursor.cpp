/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "WaitingCursor.h"

#include <QApplication>
#include <QCursor>

WaitingCursor::WaitingCursor(void)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
}

WaitingCursor::~WaitingCursor(void)
{
	QApplication::restoreOverrideCursor();
}
