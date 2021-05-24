/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "HolyServicesMF.h"

#include "../preferences/UserSettings.h"
#include "../output/CsvExporter.h"
#include "../output/XmlExporter.h"
#include "../output/HtmlExporter.h"
#include "../output/PrinterExporter.h"
#include "../output/CalendarExporter.h"
#include "../output/OfficeTableExporter.h"
#include "../output/OpenOfficeExporter.h"

#include "IPersistentStore.h"

#include "ButtonDecorator.h"
#include "Priest.h"
#include "Client.h"
#include "ClientIndexing.h"
#include "ClientOverview.h"
#include "ChurchEditor.h"
#include "ClientFilter.h"
#include "HolyService.h"
#include "HolyServiceIndexing.h"
#include "PriestIndexing.h"
#include "ChurchIndexing.h"
#include "PersistentStoreFactory.h"
#include "ServiceCalendarWidget.h"
#include "functors.h"
#include "CurrentPersistentStore.h"
#include "ServiceGeneratorDlg.h"
#include "ImportCsv.h"
#include "WaitingCursor.h"
#include "AboutDlg.h"
#include "OutputOptionsDlg.h"
#include "NewFileWizard.h"
#include "UpdateGuard.h"
#include "ArchivingTask.h"
#include "HolidayListModel.h"
#include "HolidayManagementDlg.h"
#include "CountMoneyContributionsDlg.h"
#include "ServiceTimeShiftWizard.h"
#include "GoogleCalendarWizard.h"
#include "ImportServicesWizard.h"
#include "daterangefilter.h"

#include <QMessageBox>
#include <QSpinBox>
#include <QUndoStack>
#include <QTextBrowser>
#include <QLocale>
#include <QtDebug>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <QDialogButtonBox>

#include <algorithm>

HolyServicesMF::HolyServicesMF(void) : super()
{
	setupUi(this);

	setWindowIcon(QIcon(":/images/holy_services16.png"));
	m_helpWindow = nullptr;
	m_mruFilesAction = nullptr;
	m_mruMapper = nullptr;
    m_toolbarCombosBeingFilled = false;
    m_generatorUndo = nullptr;
    m_central = new ServiceCalendarWidget(nullptr);

    // switch between yearly and trimester view
    m_mainViewGroup = new QActionGroup(this);
    m_mainViewGroup->setExclusive(true);
    m_mainViewGroup->addAction(actionYearAtOnce);
    m_mainViewGroup->addAction(actionAsTrimesters);
    if (UserSettings::get().mainView() == UserSettings::EntireYear)
    {
        actionYearAtOnce->setChecked(true);
        mainViewAsYear();
    }
    else
    {
        actionAsTrimesters->setChecked(true);
        mainViewAsTrimesters();
    }

    setCentralWidget(m_central);

    connect(actionExit, &QAction::triggered, this, &QWidget::close);
    connect(actionPreferences, &QAction::triggered, this, &HolyServicesMF::preferences);
    connect(actionClients, &QAction::triggered, this, &HolyServicesMF::clientManagement);
    connect(actionPriests, &QAction::triggered, this, &HolyServicesMF::priestManagement);
    connect(actionChurches, &QAction::triggered, this, &HolyServicesMF::churchManagement);
    connect(actionChurch_Detail_Settings, &QAction::triggered, this, &HolyServicesMF::churchEditor);
    connect(actionHoly_service_generator, &QAction::triggered,
            this, &HolyServicesMF::serviceGenerator);
    connect(actionShift_Services_In_Time, &QAction::triggered,
            this, &HolyServicesMF::shiftServicesInTime);
    connect(actionSave_all, &QAction::triggered,
            this, &HolyServicesMF::saveAll);
    connect(actionAbout, &QAction::triggered, this, &HolyServicesMF::about);
    connect(actionPrint, &QAction::triggered, this, &HolyServicesMF::print);
    connect(actionHelp_index, &QAction::triggered, this, &HolyServicesMF::helpIndex);
    connect(actionNew_file, &QAction::triggered, this, &HolyServicesMF::newFile);
    connect(actionOpen_file, &QAction::triggered, this, &HolyServicesMF::openFile);
    connect(actionHolidays, &QAction::triggered, this, &HolyServicesMF::holidayManagement);
    connect(actionCount_up_stipendia, &QAction::triggered, this, &HolyServicesMF::countMoneyContributions);
    connect(actionClient_assigned, &QAction::triggered, this, &HolyServicesMF::assignedWhenClientIsAssigned);
    connect(actionIntention_is_filled, &QAction::triggered, this, &HolyServicesMF::assignedWhenIntentionIsFilled);
    connect(menuFile, &QMenu::aboutToShow, this, &HolyServicesMF::addMru);
    connect(actionYearAtOnce, &QAction::triggered, this, &HolyServicesMF::mainViewAsYear);
    connect(actionAsTrimesters, &QAction::triggered, this, &HolyServicesMF::mainViewAsTrimesters);

	equipToolbar();

    // apply assignment criteria
    m_assignedCriteriaGroup = new QActionGroup(this);
    m_assignedCriteriaGroup->setExclusive(true);
    m_assignedCriteriaGroup->addAction(actionClient_assigned);
    m_assignedCriteriaGroup->addAction(actionIntention_is_filled);
    if (UserSettings::get().isClientImportant())
    {
        actionClient_assigned->setChecked(true);
    }
    else
    {
        actionIntention_is_filled->setChecked(true);
    }
    m_central->updateAllOccupied();

    // define what should be disabled on closed file
	m_actionsActiveOnOpenedFile << actionClients << actionPriests << actionChurches <<
		actionHoly_service_generator << actionSave_all << actionPrint;
	m_widgetsActiveOnOpenedFile << m_central << 
		m_yearSpin << m_priestCombo << m_churchCombo;

	// disable everything that should be disabled
	closeFile();

	// look for some file to be opened on startup
	const QString defaultFile = defaultFileName();

	if (! defaultFile.isEmpty())
	{
        openParticularFile(defaultFile);
	}
}

HolyServicesMF::~HolyServicesMF(void)
{
    UserSettings::get().setDefaultYear(displayedYear());
    UserSettings::get().write();

	closeFile();
}

void HolyServicesMF::closeEvent(QCloseEvent* e)
{
	if (m_helpWindow)
	{ 
		delete m_helpWindow;
	}
	super::closeEvent(e);
}

void HolyServicesMF::newFile()
{
    NewFileWizard wizard(this);

    if (wizard.exec() != QDialog::Accepted)
        return;
    const WaitingCursor sandGlass;

    closeFile();

    QString errMsg;
    const QString nameForNewFile = wizard.field("newFileName").toString();
    IPersistentStore *pNewStore = PersistentStoreFactory::createPersistentStore(
                PersistentStoreFactory::XML_DOM_STORAGE,
                nameForNewFile, &errMsg);

    if (pNewStore)
    {
        Q_ASSERT(CurrentPersistentStore::get() == nullptr);
        CurrentPersistentStore::set(pNewStore);

        HolyService *pFirstService = new HolyService;

        // insert church info
        {
            Church *pChurch = new Church;

            pChurch->setName(wizard.field("churchName").toString());
            pChurch->setPlace(wizard.field("churchPlace").toString());
            pChurch->setId(IdTag::createInitialized());

            ChurchIndexing::allSet().addElement(pChurch);

            pFirstService->setChurch(pChurch);
        }
        // priest info from wizard
        {
            Priest *pPriest = new Priest;

            pPriest->setFirstName(wizard.field("priestFirstName").toString());
            pPriest->setSurname(wizard.field("priestSurname").toString());
            pPriest->setId(IdTag::createInitialized());

            PriestIndexing::allSet().addElement(pPriest);

            pFirstService->setPriest(pPriest);
        }
        // client data
        {
            Client *pClient = new Client;

            pClient->setFirstName(wizard.field("clientFirstName").toString());
            pClient->setSurname(wizard.field("clientSurname").toString());
            pClient->setId(IdTag::createInitialized());

            ClientIndexing::allSet().addElement(pClient);

            pFirstService->setClient(pClient);
        }
        // fill in missing data into a first service record
        {
            QDateTime startDT;

            startDT.setTime(wizard.field("serviceTime").toTime());
            startDT.setDate(wizard.field("serviceDate").toDate());

            pFirstService->setStartTime(startDT);
            // make the end time by default 1 hour after start
            pFirstService->setEndTime(startDT.addSecs(3600));
            pFirstService->setIntention(wizard.field("serviceIntention").toString());
            pFirstService->setId(IdTag::createInitialized());

            // no conflict can happen in empty file
            HolyService *pConflictingDummy;
            HolyServiceIndexing::allSet().addElement(pFirstService, pConflictingDummy);
        }
        // save the new data
        saveAll();

        // everything is ready - call the internal procedure to show our new file
        openFileInternal();

        // set year to the service date to have the service immediately visible
        m_yearSpin->setValue(wizard.field("serviceDate").toDate().year());
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), errMsg);
    }
}

void HolyServicesMF::openFile()
{
	// until version 1.0.0.11 only file with this name was opened
	const QString legacyDefaultFileName("HolyServices.dat");
	const QString fName = QFileDialog::getOpenFileName(this, 
		tr("Open file with holy services"), QString(),
		tr("Holy services files (*.%1 *%2);;All files (*.*)").
        arg(UserSettings::defaultFileSuffix(), legacyDefaultFileName));

	if (! fName.isEmpty())
	{
        openParticularFile(QDir::toNativeSeparators(fName));
	}
}

void HolyServicesMF::openParticularFile(const QString& fileName)
{
	// refuse to open the same file twice
    if (CurrentPersistentStore::get() &&
        CurrentPersistentStore::get()->identification() == fileName)
	{
		QMessageBox::information(this, tr("File is already opened"),
			tr("File %1\n is already opened by this program.").arg(fileName));
		return;
	}

	// close previously opened file
	closeFile();

	// and go to newly selected
	QString errMsg;
	{
        const WaitingCursor hourGlass;
        CurrentPersistentStore::set(PersistentStoreFactory::createPersistentStore(
            PersistentStoreFactory::XML_DOM_STORAGE,
			fileName, &errMsg));
	}

    if (CurrentPersistentStore::get())
	{
        const WaitingCursor hourGlass;
		{
			IPersistentStore::tHolyServiceList services;

            CurrentPersistentStore::get()->loadAllHolyServices(services);
			HolyServiceIndexing::allSet().addElements(services);
		}
		{
			IPersistentStore::tClientList clients;

            CurrentPersistentStore::get()->loadAllClients(clients);
			ClientIndexing::allSet().addElements(clients);
		}
		{
			IPersistentStore::tPriestList priests;

            CurrentPersistentStore::get()->loadAllPriests(priests);
            PriestIndexing::allSet().addElements(priests);
		}
		{
			IPersistentStore::tChurchList churches;

            CurrentPersistentStore::get()->loadAllChurches(churches);
            ChurchIndexing::allSet().addElements(churches);
		}

		openFileInternal();
    }
	else
	{
		// if it is in recently used files, remove the file record
        UserSettings::get().removeRecentlyUsedFile(fileName);

		QMessageBox::critical(this, tr("Critical error"), 
			tr("The persistent store cannot be initialized:\n%1").arg(errMsg));
	}
}

void HolyServicesMF::openFileInternal()
{
	// the changes on loading the data, clearing and refilling filtering combos 
	// can cause the unpleasant clashes and accesses on not-yet existing filters
	// and structures - shall you uncomment the guard, you might encounter
	// crash on switching the data files
    const UpdateGuard updateGuard(this);

	// add to recently used files and to window title
    const QString storeId = CurrentPersistentStore::get()->identification();
	// for all currently implemented stores the store ID is a file path
    const QString canonicalFileName = QDir::toNativeSeparators(
		QFileInfo(storeId).absoluteFilePath());

    UserSettings::get().addRecentlyUsedFile(canonicalFileName);
	setWindowTitle(tr("Holy Services [%1]").arg(canonicalFileName));

	/// fill filter combos
	fillPriestCombo();
	fillChurchCombo();

	/// set defaults from registry
    m_yearSpin->setValue(UserSettings::get().defaultYear());

	{
        Priest* p = UserSettings::get().defaultPriest();

		if (p)
		{
			m_priestCombo->setCurrentIndex(
				m_priestCombo->findData(p->getId().toString()));
		}
	}
	{
        Church* p = UserSettings::get().defaultChurch();

		if (p)
		{
			m_churchCombo->setCurrentIndex(
				m_churchCombo->findData(p->getId().toString()));
		}
	}

	// enable the actions and widgets
	activateFileRelatedStuff(true);
}

void HolyServicesMF::closeFile()
{
    if (CurrentPersistentStore::get())
	{
		saveAll();

		HolyServiceIndexing::releaseAllSet();
		ClientIndexing::releaseAllSet();
        PriestIndexing::releaseAllSet();
        ChurchIndexing::releaseAllSet();

        CurrentPersistentStore::get()->close();
        PersistentStoreFactory::destroyPersistentStore(CurrentPersistentStore::get());
        CurrentPersistentStore::set(nullptr);
	}

    delete m_generatorUndo; m_generatorUndo = nullptr;

	activateFileRelatedStuff(false);
}

void HolyServicesMF::saveAll(void)
{
    const WaitingCursor hourGlass;

	{
		tHolyServiceVector allSet = HolyServiceIndexing::allSet().data();

		std::for_each(allSet.begin(), allSet.end(), 
            SaveModifiedService(CurrentPersistentStore::get()));
	}

	{
		tClientVector allClients = ClientIndexing::allSet().data();

		std::for_each(allClients.begin(), allClients.end(), 
            SaveModifiedClient(CurrentPersistentStore::get()));
	}

	{
        tPriestVector allPriests = PriestIndexing::allSet().data();

		std::for_each(allPriests.begin(), allPriests.end(), 
            SaveModifiedPriest(CurrentPersistentStore::get()));
	}

	{
        tChurchVector allChurches = ChurchIndexing::allSet().data();

		std::for_each(allChurches.begin(), allChurches.end(), 
            SaveModifiedChurch(CurrentPersistentStore::get()));
	}
}

void HolyServicesMF::clientManagement()
{
	ClientOverview dlg(this, ClientOverview::CLIENT);

	dlg.exec();

	// remove duplicities created
	if (ClientIndexing::hasDuplicities(ClientIndexing::allSet()))
	{
		tClientVector duplicities = 
			ClientIndexing::findDuplicities(ClientIndexing::allSet(), true);

        foreach(Client* pClient, duplicities)
		{
			// remove from database
            if (CurrentPersistentStore::get()->deleteClient(pClient))
			{
				HolyServiceIndexing::allSet().removeClient(pClient);
				ClientIndexing::allSet().deleteElement(pClient);
				delete pClient;
			}
		}

		QMessageBox::information(this, tr("Information"),
			tr("%1 unused duplicated record(s) was (were) deleted.").arg(duplicities.size()));
	}
}

void HolyServicesMF::priestManagement()
{
	ClientOverview dlg(this, ClientOverview::PRIEST);

	dlg.exec();

	// remove duplicities created
    if (PriestIndexing::hasDuplicities(PriestIndexing::allSet()))
	{
		tPriestVector duplicities = 
            PriestIndexing::findDuplicities(PriestIndexing::allSet(), true);

        foreach(Priest* pPriest, duplicities)
		{
			// remove from database
            if (CurrentPersistentStore::get()->deletePriest(pPriest))
			{
				HolyServiceIndexing::allSet().removePriest(pPriest);
                PriestIndexing::allSet().deleteElement(pPriest);
				delete pPriest;
			}
		}

		QMessageBox::information(this, tr("Information"),
			tr("%1 unused duplicated record(s) was (were) deleted.").arg(duplicities.size()));
	}

	fillPriestCombo();
}

void HolyServicesMF::churchManagement()
{
	ClientOverview dlg(this, ClientOverview::CHURCH);

	dlg.exec();

	// remove duplicities created
    if (ChurchIndexing::hasDuplicities(ChurchIndexing::allSet()))
	{
		tChurchVector duplicities = 
            ChurchIndexing::findDuplicities(ChurchIndexing::allSet(), true);

        foreach(Church* pChurch, duplicities)
		{
			// remove from database
            if (CurrentPersistentStore::get()->deleteChurch(pChurch))
			{
				HolyServiceIndexing::allSet().removeChurch(pChurch);
                ChurchIndexing::allSet().deleteElement(pChurch);
				delete pChurch;
			}
		}

		QMessageBox::information(this, tr("Information"),
			tr("%1 unused duplicated record(s) was (were) deleted.").arg(duplicities.size()));
	}

	fillChurchCombo();
}

void HolyServicesMF::churchEditor()
{
    ChurchEditor dlg(this);

	if (dlg.exec() == QDialog::Accepted)
	{
		fillChurchCombo();
	}
}

void HolyServicesMF::preferences()
{
	//CImportCsv importer;
	//QString reportAboutImport;

	//importer.import("kostely.csv", "knezi.csv", "farnici.csv", "mse.csv", reportAboutImport);
	//
	//QMessageBox::information(this, tr("Information"), reportAboutImport);
}

void HolyServicesMF::serviceGenerator()
{
    if (m_generatorUndo == nullptr)
    {
        m_generatorUndo = new QUndoStack(this);
    }

    ServiceGeneratorDlg dlg(this, displayedYear());

    dlg.setUndo(m_generatorUndo);

	dlg.exec();
	saveAll();
    m_central->updateAllOccupied();
}

void HolyServicesMF::shiftServicesInTime()
{
    const QDateTime now = QDateTime::currentDateTime();
    const tHolyServiceVector intencesFromForm =
        HolyServiceIndexing::allSet().query(
                now.addYears(-1).date(), now.addYears(+1).date(),
        m_central->currentFilter());
    if (intencesFromForm.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("There are no services available to be shifted."));
        return;
    }
    ServiceTimeShiftWizard timeShiftWizard(this);
    timeShiftWizard.setData(intencesFromForm);

    if (timeShiftWizard.exec() == QDialog::Accepted)
    {
        if (timeShiftWizard.servicesToBeShifted().isEmpty() ||
                (timeShiftWizard.wantedTimeShift().count() == 0)) return;
        foreach(HolyService* shiftedService, timeShiftWizard.servicesToBeShifted())
        {
            shiftedService->moveInTime(timeShiftWizard.wantedTimeShift());
        }
        saveAll();
    }
}

void HolyServicesMF::equipToolbar()
{
    toolBar->addSeparator();
	m_yearSpin = new QSpinBox(nullptr);
    m_yearSpin->setRange(1997, 2200);
	m_yearSpin->setPrefix(tr("year "));
	m_yearSpin->setToolTip(tr("Services for this year are displayed"));
	toolBar->addWidget(m_yearSpin);
	connect(m_yearSpin, SIGNAL(valueChanged(int)), SLOT(changeYear(int)));

    toolBar->addSeparator();
    m_priestCombo = new QComboBox(toolBar);
	m_priestCombo->setToolTip(tr("Display services of selected priest"));
	fillPriestCombo();
	connect(m_priestCombo, SIGNAL(currentIndexChanged(int)), SLOT(priestComboChanged(int)));
	toolBar->addWidget(m_priestCombo);

    toolBar->addSeparator();
    m_churchCombo = new QComboBox(toolBar);
	m_churchCombo->setToolTip(tr("Display services of selected church"));
	fillChurchCombo();
	connect(m_churchCombo, SIGNAL(currentIndexChanged(int)), SLOT(churchComboChanged(int)));
	toolBar->addWidget(m_churchCombo);
}

void HolyServicesMF::changeYear(int newYear)
{
	m_central->setYear(newYear);
}

int HolyServicesMF::displayedYear() const
{
	return m_yearSpin->value();
}

void HolyServicesMF::priestComboChanged(int /*index*/)
{
    if (m_toolbarCombosBeingFilled)
    {
        return;
    }
    
	{
        ServiceCalendarWidget::EFilterOptions priestOption = ServiceCalendarWidget::ALL;
		const int priestIndex = m_priestCombo->currentIndex();
		QVariant data = m_priestCombo->itemData(priestIndex);
		IdTag priestId;

		if (data.type() == QVariant::String)
		{
			priestId = IdTag::createFromString(data.toString());

			{
                Priest *p = PriestIndexing::allSet().find(priestId);

                UserSettings::get().setDefaultPriest(p);
			}

            priestOption = ServiceCalendarWidget::PARTICULAR;
			Q_ASSERT(priestId.isValid());
		}
		else if (data.type() == QVariant::Int)
		{
            priestOption = ServiceCalendarWidget::ALL;
                        UserSettings::get().setDefaultPriest(nullptr);
                }
		m_central->setPriestFilter(priestOption, priestId);
	}

    const WaitingCursor hourGlass;
	m_central->propagateFilter();
}

void HolyServicesMF::churchComboChanged( int /*index*/ )
{
    if (m_toolbarCombosBeingFilled)
    {
        return;
    }

    {
        ServiceCalendarWidget::EFilterOptions churchOption = ServiceCalendarWidget::ALL;
        const int churchIndex = m_churchCombo->currentIndex();
        QVariant data = m_churchCombo->itemData(churchIndex);
        IdTag churchId;

        if (data.type() == QVariant::String)
        {
            churchId = IdTag::createFromString(data.toString());

            {
                Church *p = ChurchIndexing::allSet().find(churchId);

                UserSettings::get().setDefaultChurch(p);
            }

            churchOption = ServiceCalendarWidget::PARTICULAR;
            Q_ASSERT(churchId.isValid());
        }
        else if (data.type() == QVariant::Int)
        {
            churchOption = ServiceCalendarWidget::ALL;
            UserSettings::get().setDefaultChurch(nullptr);
        }
        m_central->setChurchFilter(churchOption, churchId);
    }

    const WaitingCursor hourGlass;
    m_central->propagateFilter();
}

void HolyServicesMF::fillPriestCombo()
{
    m_toolbarCombosBeingFilled = true;
	m_priestCombo->clear();
    m_priestCombo->addItem(tr("<All priests>"), QVariant(ServiceCalendarWidget::ALL));

    tPriestVector priests = PriestIndexing::allSet().data();
    Priest* pPriest;

	foreach(pPriest, priests)
	{
		m_priestCombo->addItem(pPriest->toString(), QVariant(pPriest->getId().toString()));
	}
    m_toolbarCombosBeingFilled = false;
}

void HolyServicesMF::fillChurchCombo()
{
	m_toolbarCombosBeingFilled = true;
    m_churchCombo->clear();
    m_churchCombo->addItem(tr("<All churches>"), QVariant(ServiceCalendarWidget::ALL));

    QPixmap churchColorDot(16, 16);
    const tChurchVector churches = ChurchIndexing::allSet().data();

    for(const Church* pChurch : churches)
	{
		churchColorDot.fill(pChurch->color());
		m_churchCombo->addItem(churchColorDot, pChurch->toString(), QVariant(pChurch->getId().toString()));
	}
    m_toolbarCombosBeingFilled = false;
}

void HolyServicesMF::about()
{
    AboutDlg(this).exec();
}

void HolyServicesMF::helpIndex()
{
	if (m_helpWindow == nullptr)
	{ 
        QDir appSharedFolder(UserSettings::appSharedDataPath());
		// look for the help
		// preferably country specific
		const QString basicName("HolyServices");
		const QString localeName(QLocale().name());
		const QString helpSuffix("html");
        const QString localeFile = appSharedFolder.absoluteFilePath(
                    QString("%1_%2.%3").arg(basicName, localeName, helpSuffix));
        const QString standardFile = appSharedFolder.absoluteFilePath(
                    QString("%1.%2").arg(basicName, helpSuffix));
		QUrl helpSource;

		if (QFile::exists(localeFile))
		{
			helpSource = QUrl::fromLocalFile(localeFile);
		}
		else if (QFile::exists(standardFile))
		{ // fallback to default - english help
			helpSource = QUrl::fromLocalFile(standardFile);
		}
		else
		{
			QMessageBox::warning(this, tr("Error"), 
				tr("Help file (%1, %2) cannot be found\nin %3.").
				arg(standardFile, localeFile, 
                QDir::toNativeSeparators(QDir::currentPath())));
			return;
		}

		// create and lay out the help window
		m_helpWindow = new QWidget(nullptr);
		m_helpWindow->setAttribute(Qt::WA_DeleteOnClose);
		m_helpWindow->setWindowIcon(QIcon(":images/help.png"));

		new QVBoxLayout(m_helpWindow);

		QTextBrowser* pBrowser = new QTextBrowser(m_helpWindow);

		m_helpWindow->layout()->addWidget(pBrowser);

		QDialogButtonBox* buttonBox = new QDialogButtonBox(m_helpWindow);

		buttonBox->setStandardButtons(QDialogButtonBox::Close);
		ButtonDecorator::assignIcons(*buttonBox);
		connect(buttonBox, SIGNAL(rejected()), m_helpWindow, SLOT(close()));
		m_helpWindow->layout()->addWidget(buttonBox);

		pBrowser->setSource(helpSource);

		// initially smaller than the main window
		m_helpWindow->resize(size() * 0.7);
	}

	if (m_helpWindow->isMinimized())
	{
		m_helpWindow->showNormal();
	}
	m_helpWindow->show();
}

void HolyServicesMF::print()
{
    OutputOptionsDlg dlg(this);

    dlg.setYear(UserSettings::get().defaultYear());
    dlg.setHolidays(m_central->holidayProvider());

    if (dlg.exec() != QDialog::Accepted) return;

    QDate from, to;
    OutputOptionsDlg::EOutputFormat format = OutputOptionsDlg::UnknownFormat;
    OutputOptionsDlg::EOutputContents contents = OutputOptionsDlg::JustServices;
    QString clientIdString;

    dlg.get(from, to, clientIdString, format, contents);

    const tHolyServiceVector intencesFromForm =
        HolyServiceIndexing::allSet().query(from, to,
        m_central->currentFilter());

    tHolyServiceVector forExport;

    if (clientIdString == QString("*"))
    {
        forExport = intencesFromForm; // no client filtering required
    }
    else
    {
        const IdTag clientId = IdTag::createFromString(clientIdString);
        const ClientFilter clientFilter(clientId);

        forExport = HolyServiceIndexing::filter(intencesFromForm, &clientFilter);
    }

    if (forExport.isEmpty())
    {
        QMessageBox::information(this, tr("Information"),
            tr("No holy service was found according to given search conditions."));
        return;
    }

    IExporter* plainExporter = nullptr;
    QString exportFile = QDateTime::currentDateTime().toString(Qt::ISODate);
    exportFile.append(tr("Holy_services",
        "translate without diacritic signs or the auto-spawn"
        "of related application stops working"));

    switch (format)
    {
    case OutputOptionsDlg::CsvFormat:
        plainExporter = new CsvExporter("System",
            UserSettings::get().reportColumns());
        exportFile.append(".csv");
        break;
    case OutputOptionsDlg::XmlFormat:
        plainExporter = new XmlExporter;
        exportFile.append(".xml");
        break;
    case OutputOptionsDlg::NativePrinterFormat:
        plainExporter = new PrinterExporter(QPrinter::NativeFormat,
            UserSettings::get().reportColumns());
        exportFile.append(".printer"); // export file name will not be used
        break;
    case OutputOptionsDlg::PdfFormat:
        plainExporter = new PrinterExporter(QPrinter::PdfFormat,
            UserSettings::get().reportColumns());
        exportFile.append(".pdf");
        break;
    case OutputOptionsDlg::HtmlFormat:
        plainExporter = new HtmlExporter(UserSettings::get().reportColumns());
        exportFile.append(".html");
        break;
    case OutputOptionsDlg::iCalFormat:
        plainExporter = new CalendarExporter;
        exportFile.append(".ics");
        break;
    case OutputOptionsDlg::OfficeClipboardFormat:
        plainExporter = new OfficeTableExporter(UserSettings::get().reportColumns());
        // no export file
        break;
    case OutputOptionsDlg::OpenOfficeWriterFormat:
        plainExporter = new OpenOfficeExporter(UserSettings::get().reportColumns());
        exportFile.append(".odt");
        break;
    default:
        Q_ASSERT(0);
        break;
    }

    exportFile.replace(':', '-');

    // do the export to the temp folder
    const QFileInfo exportFileInfo(QDir::tempPath(), exportFile);
    exportFile = exportFileInfo.absoluteFilePath();

    if (! plainExporter) return;
    std::unique_ptr<IExporter> exporter(plainExporter);

    if (! exporter->init(exportFile, QString()))
    {
        QMessageBox::warning(this, tr("Error"),
            tr("Error on initialization of file\n%1").arg(exportFile));
        return;
    }

    const WaitingCursor hourGlass;
    exporter->setHolidayProvider(m_central->holidayProvider());

    switch (contents)
    {
    case OutputOptionsDlg::JustServices:
        for(const HolyService * const p : forExport)
        {
            exporter->write(*p, 0);
        }
        break;
    case OutputOptionsDlg::IncludeBlankDates:
        {
            QDate exportedDay = from;
            while (exportedDay <= to)
            {
                DateRangeFilter oneDayFilter;
                oneDayFilter.setRange(exportedDay, exportedDay);
                auto oneDayExport = HolyServiceIndexing::filter(forExport, &oneDayFilter);
                if (oneDayExport.isEmpty())
                {
                    exporter->writeBlankLine(exportedDay);
                }
                else
                {
                    for(const HolyService * const p : oneDayExport)
                    {
                        exporter->write(*p, 0);
                    }
                }

                exportedDay = exportedDay.addDays(1);
            }
        }
        break;
    }

    exporter->close();

    const QSet<OutputOptionsDlg::EOutputFormat> formatsNotProducingFile({
        OutputOptionsDlg::NativePrinterFormat, OutputOptionsDlg::OfficeClipboardFormat
    });
    if (! formatsNotProducingFile.contains(format) )
    {
        // open in proper app
        const QFileInfo fi(exportFile);
        const QString absoluteFile = fi.absoluteFilePath();

        if (! QDesktopServices::openUrl(QUrl::fromLocalFile(absoluteFile)))
        {
            QMessageBox::information(this, tr("Information"),
                tr("File %1\nwas successfully exported, however your system\n"
                "does not have a default action for this type of data.\n\n"
                "Please, open the data manually").arg(absoluteFile));
        }
    }
}

QString HolyServicesMF::defaultFileName()
{
	// command line
	{
		const QString fileParamKey("--file");
		const QStringList commandLine = qApp->arguments();
		const int fileParamPosition = commandLine.indexOf(fileParamKey);

		if (fileParamPosition >= 0)
		{
			if (fileParamPosition == (commandLine.size() - 1))
			{
				QMessageBox::warning(this, tr("Parameter value unknown"),
					tr("You have requested to open a file by <b>--file</b> parameter,\n"
					"but you haven't specified any file to be opened."));

				// parameter was given but nothing in - do not open MRU file 
				// in order not to confuse user pretending that on the end the file was found
				return QString();
			}
			else
			{
				const QString requestedFile = commandLine.at(fileParamPosition + 1);

				if (QFile::exists(requestedFile))
				{
					// desired way out
					return requestedFile;
				}
				else
				{
					QMessageBox::warning(this, tr("File cannot be opened"),
						tr("You have requested to open a file <tt>%1</tt><br>"
						"by <b>--file</b> parameter,<br>"
						"but the file does not exist.").arg(requestedFile));

					// parameter was given but nothing in - do not open MRU file 
					// in order not to confuse user pretending that on the end the file was found
					return QString();
				}
			}
		}

                // or if there is just one parameter which (by convention) is the file name to be opened
                if ((commandLine.size() == 2) && QFile::exists(commandLine.last()))
                {
                     return commandLine.last();
                }
	}

	{ // recent files
        const QStringList mruFiles = UserSettings::get().recentlyUsedFiles();
		QStringListIterator it(mruFiles);

		while (it.hasNext())
		{
			const QString mruFile = it.next();

			if (QFile::exists(mruFile))
			{
				return mruFile;
			}
			else
			{
                UserSettings::get().removeRecentlyUsedFile(mruFile);
			}
		}
	}

	// legacy one - the name that was only used in pre 1.0.0.11
	const QString legacyStorageName("storedHolyServices.dat");
	if (QFile::exists(legacyStorageName))
	{
		return legacyStorageName;
	}

	return QString();
}

void HolyServicesMF::activateFileRelatedStuff(bool enable)
{
	foreach (QAction *pAction, m_actionsActiveOnOpenedFile)
	{
		pAction->setEnabled(enable);
	}
	foreach(QWidget *pWidget, m_widgetsActiveOnOpenedFile)
	{
		pWidget->setEnabled(enable);
	}
}

void HolyServicesMF::addMru()
{
    QStringList recentlyUsedFiles = UserSettings::get().recentlyUsedFiles();

	if (recentlyUsedFiles.isEmpty())
	{
		if (m_mruFilesAction)
		{	
			delete m_mruFilesAction; m_mruFilesAction = nullptr;
			delete m_mruMapper; m_mruMapper = nullptr;
		}
	}
	else 
	{
		if (m_mruFilesAction == nullptr)
		{
			const int positionOfFirstSeparatorAfterFileOpen = 2;
			QMenu *pMruMenu = new QMenu(tr("Recently used files"), this);

			m_mruFilesAction = menuFile->insertMenu(
				menuFile->actions().at(positionOfFirstSeparatorAfterFileOpen), pMruMenu);
			m_mruMapper = new QSignalMapper(this);
			connect(m_mruMapper, SIGNAL(mapped(const QString&)), 
                SLOT(openParticularFile(const QString&)));
		}

		QMenu *mruMenu = m_mruFilesAction->menu();
		Q_ASSERT(mruMenu);

		mruMenu->clear();

		foreach(const QString& usedFile, recentlyUsedFiles)
		{
			QAction *usedFileAction = mruMenu->addAction(usedFile);

			connect(usedFileAction, SIGNAL(triggered()), m_mruMapper, SLOT(map()));
			m_mruMapper->setMapping(usedFileAction, usedFile);
		}
	}
}


void HolyServicesMF::on_actionDearchive_triggered()
{
    const QString nameFilter = tr("Archived files (*.%1);;All files (*.*)").arg(ArchivingTask::archiveFileExtension);
    const QStringList locations = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    QString defaultLocation;

    if (! locations.isEmpty())
    {
        defaultLocation = locations.first();
    }
    const QString archiveFileName = QFileDialog::getOpenFileName(this, tr("Select archived file"),
        defaultLocation, nameFilter);

    if (archiveFileName.isEmpty())
    {
        return;
    }
    QFile archive(archiveFileName);

    if (archive.open(QIODevice::ReadOnly))
    {
        const QByteArray compressed = archive.readAll();
        const QByteArray uncompressedData = qUncompress(compressed);
        const QString tempLocation = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
            + QDir::separator() + QFileInfo(archiveFileName).baseName();
        const QString uncompressedFileName = QFileDialog::getSaveFileName(this,
            tr("Where to write uncompressed file?"),
            tempLocation,
            tr("HolyServices file (*.%1)").arg(UserSettings::defaultFileSuffix()));
        if (uncompressedFileName.isEmpty())
        {
            return;
        }

        QFile uncompressedFile(uncompressedFileName);
        if (uncompressedFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            uncompressedFile.write(uncompressedData);
            uncompressedFile.close();

            const int answer = QMessageBox::question(this, tr("Open unarchived file?"),
                  tr("Archive file has been written into\n%1\n\nShould it be open now?").arg(uncompressedFileName),
                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if (answer == QMessageBox::Yes)
            {
                openParticularFile(uncompressedFileName);
            }
        }
    }
}

void HolyServicesMF::holidayManagement()
{
    HolidayManagementDlg dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        m_central->refreshHolidayProvider();    
    }
}

void HolyServicesMF::countMoneyContributions()
{
    CountMoneyContributionsDlg dlg(this);

    dlg.setFilter(m_central->currentFilter());

    dlg.exec();
}

void HolyServicesMF::on_actionGoogle_Calendar_triggered()
{
    GoogleCalendarWizard wiz(this);

    wiz.setYear(m_yearSpin->value());
    wiz.setAccessToken(m_accessToken);
    wiz.setAccessTokenValidTo(m_accessTokenValidTo);

    if (UserSettings::get().defaultPriest())
    {
        wiz.setDefaultPriestId(UserSettings::get().defaultPriest()->getId());
    }

    wiz.exec();

    m_accessToken = wiz.accessToken();
    m_accessTokenValidTo = wiz.accessTokenValidTo();
}

void HolyServicesMF::assignedWhenClientIsAssigned()
{
    UserSettings::get().setAssignedCriteria(UserSettings::ClientIsAssigned);
    m_central->updateAllOccupied();
}

void HolyServicesMF::assignedWhenIntentionIsFilled()
{
    UserSettings::get().setAssignedCriteria(UserSettings::IntentionIsFilled);
    m_central->updateAllOccupied();
}

void HolyServicesMF::on_actionImport_CSV_triggered()
{
    ImportServicesWizard wizz(this);

    wizz.setYear(displayedYear());

    wizz.exec();
}

void HolyServicesMF::mainViewAsYear()
{
    m_central->setViewType(UserSettings::EntireYear);
    UserSettings::get().setMainView(UserSettings::EntireYear);
}

void HolyServicesMF::mainViewAsTrimesters()
{
    m_central->setViewType(UserSettings::Trimesters);
    UserSettings::get().setMainView(UserSettings::Trimesters);
}
