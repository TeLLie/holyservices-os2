/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "HolyServicesMF.h"

#include "HolyServiceApp.h"
#include "CurrentPersistentStore.h"
#include "UserSettings.h"

#include <QTranslator>
#include <QLocale>

#include <QMessageBox>
#include <QtDebug>
#include <QSplashScreen>

// because of unsolved problem, when in VisualStudio program loads proper locale,
// I need to fall back to dirty hack on looking for translation
#include <QDir>
#include <QFileInfo>

/// helper unction that finds first file with name loadTranslation and 
/// returns suffix of that file
QString translationFileCheck()
{
	QString retVal;

//	 if locale is C, look for hint file
    QDir dir(UserSettings::appSharedDataPath());
	const QFileInfoList files = dir.entryInfoList(QStringList() << "loadTranslation.*");
	
	if (! files.isEmpty())
	{
		QFileInfo fi = files.first();
		const QString extension = fi.suffix();

		QLocale testInstance(extension);

		if (testInstance.language() != QLocale::C)
		{
			retVal = extension;
		}
	}
	return retVal;
}

QFile logFile(QDir::temp().absoluteFilePath("hs.log"));
bool logFileOpeningWasTried = false;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (! logFileOpeningWasTried)
    {
        logFileOpeningWasTried = true;
        logFile.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);
        logFile.write("\n********************************************************");
        logFile.write(QString("\n* %1").arg(QDateTime::currentDateTime().toString(Qt::ISODate)).toUtf8());
        logFile.write("\n********************************************************\n");
    }
    if (! logFile.isOpen()) return;

    const QString now = QDateTime::currentDateTime().toString(Qt::ISODate);
    switch (type) {
    case QtDebugMsg:
        logFile.write( QString("\n%1 Debug: %2 (%3:%4, %5)").arg(now, msg, context.file).arg(context.line).arg(context.function).toUtf8());
        break;
    case QtWarningMsg:
        logFile.write( QString("\n%1 Warning: %2 (%3:%4, %5)").arg(now, msg, context.file).arg(context.line).arg(context.function).toUtf8());
        break;
    case QtCriticalMsg:
        logFile.write( QString("\n%1 Critical: %2 (%3:%4, %5)").arg(now, msg, context.file).arg(context.line).arg(context.function).toUtf8());
        break;
    case QtFatalMsg:
        logFile.write( QString("\n%1 Fatal: %2 (%3:%4, %5)").arg(now, msg, context.file).arg(context.line).arg(context.function).toUtf8());
        abort();
    default: /*nothing*/ break;
    }
}

int main(int argc, char **argv)
{
//    qInstallMessageHandler(myMessageOutput);

	HolyServiceApp app(argc, argv);

	QSplashScreen splash(QPixmap(":/images/holy_services_splash.png"));

	splash.show();
	app.processEvents();

	// command line parameters
	{
		QStringList args = QCoreApplication::arguments();

		if (args.contains("-?") || args.contains("--help"))
		{
			QMessageBox::information(nullptr, "Information", 
				"Valid arguments are"
				"\n\t-?, --help for usage information"
				"\n\t--language <country_language> i.e. de_DE or cs_CZ"
				"\n\t--file <file with holy services>");
			return 0;
		}

		const QString langKey("--language");
		if (args.contains(langKey))
		{
			const int paramIndex = args.indexOf(langKey);

			if (paramIndex < (args.size() - 1))
			{
				const QString langParam = args.at(paramIndex + 1);
				QLocale paramLocale(langParam);

				if (paramLocale != QLocale::c())
				{
					QLocale::setDefault(paramLocale);
				}
				else
				{
					QMessageBox::information(nullptr, "Warning", 
						QString("Invalid language code %1").arg(langParam));
				}
			}
		}
		else
		{
			// get along with helper file or system locale
			//qDebug() << "Locale B4 " << QLocale().name() << " system " << QLocale::system().name();
//			QMessageBox::information(nullptr, QLocale().name(), QLocale::system().name());

			QString fileBasedLocaleName = translationFileCheck();

			if (fileBasedLocaleName.isEmpty())
			{
				QLocale::setDefault(QLocale::system());
			}
			else
			{
				QLocale::setDefault(QLocale(fileBasedLocaleName));
			}

			//qDebug() << "Locale After " << QLocale().name();
//			QMessageBox::information(nullptr, QLocale().name(), QLocale::system().name());
		}
	}

    QDir dir(UserSettings::appSharedDataPath());
    const QString appTranslation = QLocale().name();
	const QString qtTranslation = "qt_" + appTranslation;

	// just a bit of release debugging
	// QMessageBox::information(nullptr, appTranslation, QDir::currentPath());

	QTranslator appTranslator;

    appTranslator.load(dir.absoluteFilePath(appTranslation));
	app.installTranslator(&appTranslator);	

	QTranslator qtTranslator;

    qtTranslator.load(dir.absoluteFilePath(qtTranslation));
	app.installTranslator(&qtTranslator);	
	
	HolyServicesMF mainWindow;
	mainWindow.show();

	splash.finish(&mainWindow);

	// run new file wizard if no file has been opened
	if (CurrentPersistentStore::get() == nullptr)
	{
		const int answer = QMessageBox::question(&mainWindow, QObject::tr("Create new file?"),
			QObject::tr("No file with holy services was found,\n"
			"would you like to create new one with help of guide?"),
			QMessageBox::Yes | QMessageBox::No);

		if (answer == QMessageBox::Yes)
		{
			mainWindow.newFile();
		}
	}

	return app.exec();
}
