/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "HolyServiceApp.h"

#include <QtDebug>
#include <QMessageBox>
#include <stdexcept>

HolyServiceApp::HolyServiceApp(int& argc, char** argv) : super(argc, argv)
{
}

HolyServiceApp::~HolyServiceApp()
{
}

bool HolyServiceApp::notify(QObject *receiver, QEvent *event)
{
	try 
	{
		return super::notify(receiver, event);
	}
	catch(std::exception& e)
	{
		QMessageBox::warning(nullptr, tr("Program exception"), 
			tr("Message:\n%1").arg(QString::fromUtf8(e.what())));
	}
	return false;
}
