/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "NewFileWizard.h"

#include "../preferences/UserSettings.h"

#include "WaitingCursor.h"

#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateEdit>
#include <QTimeEdit>
#include <QTextEdit>

NewFileWizard::NewFileWizard(QWidget *p) : super(p)
{
	setWindowTitle(tr("Guide through creation of new file"));

	addPage(createWelcomePage());
	addPage(createFileSearchPage());
	addPage(createHomeChurchPage());
	addPage(createPriestPage());
	addPage(createClientPage());
	addPage(createServicePage());
}

NewFileWizard::~NewFileWizard()
{
}

QWizardPage* NewFileWizard::createWelcomePage() const
{
	QWizardPage *page = new QWizardPage;
	QLabel *label = new QLabel(tr("This wizard will help you create "
		"new file with holy services and will lead you through initial steps.<br><br>"
		"You shall<ul><li>find proper place for your data file</li>"
		"<li>put in the brief description of your church, priest and one parish member</li>"
		"<li>and define your first holy service</li></ul>"));

	page->setTitle(tr("Introduction"));
	label->setWordWrap(true);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(label);
	page->setLayout(layout);

	return page;
}

QWizardPage* NewFileWizard::createFileSearchPage() const
{
    return new ServiceFilePage;
}

QWizardPage* NewFileWizard::createHomeChurchPage() const
{
    return new ChurchPage;
}

QWizardPage* NewFileWizard::createPriestPage() const
{
    return new PriestPage;
}

QWizardPage* NewFileWizard::createClientPage() const
{
    return new ClientPage;
}

QWizardPage* NewFileWizard::createServicePage() const
{
    return new FirstServicePage;
}

// ---------------------------------------------------------------------------
// class CChurchPage
// ---------------------------------------------------------------------------
ChurchPage::ChurchPage(QWidget* p) : super(p)
{
	setTitle(tr("Home church"));
	setSubTitle(tr("Please enter data describing the main church of your parish."));

	QBoxLayout *layout = new QVBoxLayout(this);

	layout->addWidget(new QLabel(tr("Name of the church"), this));
	
	{
		QLineEdit* churchNameEdit = new QLineEdit(this);

		churchNameEdit->setToolTip(tr("Name of your home (main) church - like church of St. Paul"));
		registerField("churchName*", churchNameEdit);
		layout->addWidget(churchNameEdit);
	}
	layout->addSpacing(10);
	{
		layout->addWidget(new QLabel(tr("Place of the church"), this));
	
		QLineEdit* churchPlaceEdit = new QLineEdit(this);

		churchPlaceEdit->setToolTip(tr("Place of your church (London)"));
		registerField("churchPlace", churchPlaceEdit);
		layout->addWidget(churchPlaceEdit);
	}
}

ChurchPage::~ChurchPage()
{
}

void ChurchPage::initializePage()
{
	// defaults
	setField("churchName", tr("Our Church"));
	setField("churchPlace", tr("Our place"));
}

// ---------------------------------------------------------------------------
// class CPriestPage
// ---------------------------------------------------------------------------
PriestPage::PriestPage(QWidget* p) : super(p)
{
	setTitle(tr("Our priest"));
	setSubTitle(tr("Please enter data describing the priest of your parish that celebrates holy services most often."));

	QBoxLayout *layout = new QVBoxLayout(this);

	layout->addWidget(new QLabel(tr("Front title and first name of the priest"), this));
	
	{
		QLineEdit* priestNameEdit = new QLineEdit(this);

		priestNameEdit->setToolTip(tr("For instance Rd. John"));
		registerField("priestFirstName*", priestNameEdit);
		layout->addWidget(priestNameEdit);
	}
	layout->addSpacing(10);
	{
		layout->addWidget(new QLabel(tr("Family name of the priest"), this));
	
		QLineEdit* priestSurname = new QLineEdit(this);

		priestSurname->setToolTip(tr("Family name and back title"));
		registerField("priestSurname", priestSurname);
		layout->addWidget(priestSurname);
	}
}

PriestPage::~PriestPage() {}

void PriestPage::initializePage()
{
	// defaults
	setField("priestFirstName", tr("John"));
	setField("priestSurname", tr("Smith"));
}

// ---------------------------------------------------------------------------
// class CClientPage
// ---------------------------------------------------------------------------
ClientPage::ClientPage(QWidget* p) : super(p)
{
	setTitle(tr("Our client"));
	setSubTitle(tr("Please enter data describing the some client from your parish - for instance you."));

	QBoxLayout *layout = new QVBoxLayout(this);

	layout->addWidget(new QLabel(tr("First name of the client"), this));
	
	{
		QLineEdit* clientNameEdit = new QLineEdit(this);

		clientNameEdit->setToolTip(tr("Your first name will do the trick"));
		registerField("clientFirstName", clientNameEdit);
		layout->addWidget(clientNameEdit);
	}
	layout->addSpacing(10);
	{
		layout->addWidget(new QLabel(tr("Family name of the client"), this));
	
		QLineEdit* clientSurname = new QLineEdit(this);

		clientSurname->setToolTip(tr("Family name (mandatory)"));
		registerField("clientSurname*", clientSurname);
		layout->addWidget(clientSurname);
	}
}

ClientPage::~ClientPage() {}

void ClientPage::initializePage()
{
	// defaults
	setField("clientFirstName", tr("Joseph"));
	setField("clientSurname", tr("Brown", "some typical family name"));
}
// ---------------------------------------------------------------------------
// class CFirstServicePage
// ---------------------------------------------------------------------------
FirstServicePage::FirstServicePage(QWidget* p) : super(p)
{
	setTitle(tr("First holy service"));
	setSubTitle(tr("Please put in time and intention for the first holy service in new file."));

	QBoxLayout *layout = new QVBoxLayout(this);

	{
		QGridLayout* pTimeGridLayout = new QGridLayout;

		pTimeGridLayout->addWidget(new QLabel(tr("Date"), this), 0, 0);
		
		{
			QDateEdit* dateEdit = new QDateEdit(this);

			dateEdit->setToolTip(tr("Date of your holy service"));
			dateEdit->setCalendarPopup(true);
			registerField("serviceDate", dateEdit, "date");
			pTimeGridLayout->addWidget(dateEdit, 1, 0);
		}

		pTimeGridLayout->addWidget(new QLabel(tr("Begins at"), this), 0, 1);

		{
			QTimeEdit* timeEdit = new QTimeEdit(this);

			timeEdit->setToolTip(tr("Starting time for the holy service"));
			timeEdit->setDisplayFormat("hh:mm");
			registerField("serviceTime", timeEdit, "time");
			pTimeGridLayout->addWidget(timeEdit, 1, 1);
		}
		// press the date and time to the left
		pTimeGridLayout->addItem(
			new QSpacerItem(20,10, QSizePolicy::Expanding, QSizePolicy::Maximum),
			0, 2);

		layout->addLayout(pTimeGridLayout);
	}
	layout->addSpacing(10);
	{
		layout->addWidget(new QLabel(tr("Intention of the service"), this));

		QTextEdit* intentionEdit = new QTextEdit(this);

		registerField("serviceIntention*", intentionEdit, 
			"plainText", SIGNAL(textChanged()));

		layout->addWidget(intentionEdit);
	}
}

FirstServicePage::~FirstServicePage() {}

void FirstServicePage::initializePage()
{
	// defaults
	setField("serviceTime", QTime(18, 0));
	setField("serviceDate", QDate::currentDate());
	setField("serviceIntention", tr("For the blessing for our parish"));
}
// ---------------------------------------------------------------------------
// CServiceFilePage
// ---------------------------------------------------------------------------
ServiceFilePage::ServiceFilePage(QWidget* p) : super(p)
{
	QBoxLayout *pLayout = new QVBoxLayout(this);

	pLayout->addWidget(new QLabel(tr("File folder"), this));

	// line edit for path to file with folder browsing button
	{
		QBoxLayout* pFolderLayout = new QHBoxLayout;

		pLayout->addLayout(pFolderLayout);

		m_path = new QLineEdit(this);
		m_path->setToolTip(tr("Target path for new file - you must have a permission to write into the folder"));
		connect(m_path, SIGNAL(textChanged(const QString&)), SLOT(displayResult()));
		pFolderLayout->addWidget(m_path);

		// folder browsing button
		{
			QWidget* browseFolderButton = 
				new QPushButton(QPixmap(":/images/filefind.png"), QString(), this);
			browseFolderButton->setToolTip(tr("Click to select the folder"));
			connect(browseFolderButton, SIGNAL(clicked()), SLOT(browseForFolder()));
			pFolderLayout->addWidget(browseFolderButton);
		}
	}

	pLayout->addSpacing(10);

	// plain file name - without suffix
	pLayout->addWidget(new QLabel(tr("Name of the new file"), this));
	m_fileName = new QLineEdit(this);
	connect(m_fileName, SIGNAL(textChanged(const QString&)), SLOT(displayResult()));
	pLayout->addWidget(m_fileName);

	pLayout->addSpacing(10);

	// and the label demonstrating the final shape of the filename
	{
		QWidget* resultingPath = new QLabel(this);
		registerField("newFileName", resultingPath, "text");
		pLayout->addWidget(resultingPath);
	}
	
	pLayout->addSpacing(10);
	
	// explanation label about invalid file name
	{
		m_fileMessageLabel = new QLabel(this);
		m_fileMessageLabel->setStyleSheet("color:red");
		m_fileMessageLabel->setWordWrap(true);
		pLayout->addWidget(m_fileMessageLabel);
	}

	setTitle(tr("Please select a path and file name for your new file"));
	setSubTitle(tr("Actually the file will hold all data from your parish (priests, churches, parish members).\n\n"
		"You could consider to name the file with the same name as your parish has."));
}

ServiceFilePage::~ServiceFilePage() {}

void ServiceFilePage::initializePage()
{
	// defaults
	m_path->setText(QDir::toNativeSeparators(QDir::homePath()));
	m_fileName->setText(tr("MyNameForFile"));
	displayResult();
}

/// take the complete file name from the label and check that such a file can be opened
bool ServiceFilePage::isComplete() const
{
	const WaitingCursor sandGlass;
	const QString fName = field("newFileName").toString();

	m_fileMessage.clear();

	if (fName.isEmpty())
	{
		m_fileMessage = tr("File name is empty");
		return false;
	}

	bool filePathOk = false;
	QFileInfo fi(fName);

	if (! fi.dir().exists())
	{
		m_fileMessage = tr("Folder %1 is not a valid folder").
			arg(QDir::toNativeSeparators(fi.absolutePath()));
	}
	else
	{
		QFile testing(fName);

		if (testing.exists())
		{
			// we will not overwrite existing
			m_fileMessage = tr("Such a file already exists.");
			filePathOk = false;
		}
		else
		{
			filePathOk = testing.open(QIODevice::WriteOnly);

			if (filePathOk)
			{
				testing.close();
				filePathOk = testing.remove();
			}
			else
			{
				m_fileMessage = tr("You probably do not have right to create file in that folder.");
			}
		}
	}
	return filePathOk;
}

void ServiceFilePage::browseForFolder()
{
	const QString aPath = QFileDialog::getExistingDirectory(this, 
		tr("Select a folder to put new file into"), m_path->text());

	if (! aPath.isEmpty())
	{
		m_path->setText(QDir::toNativeSeparators(aPath));
	}
}

void ServiceFilePage::displayResult()
{
	const QString result = currentFileName();

	setField("newFileName", result);

	const bool newPageCompleteState = isComplete();
	
	emit completeChanged();

	m_fileMessageLabel->setVisible(! newPageCompleteState);
	m_fileMessageLabel->setText(m_fileMessage);
}

QString ServiceFilePage::currentFileName() const
{
	const QDir dir(m_path->text());
	const QFileInfo fi(dir, m_fileName->text());

	return QDir::toNativeSeparators(fi.absoluteFilePath() + 
		'.' + UserSettings::defaultFileSuffix());
}
