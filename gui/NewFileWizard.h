#ifndef NEWFILEWIZARD_H_INCLUDED_3E6B79B9_789D_49BD_A852_1FEEA56FA04E
#define NEWFILEWIZARD_H_INCLUDED_3E6B79B9_789D_49BD_A852_1FEEA56FA04E

///\class NewFileWizard
/// This wizard should guide the user through creating of new holy services 
/// file with basic information - as a first step in application for newbies
/// the wizard should be very neat and simple
///\date 3-2008
///\author Jan 'Kovis' Struhar
#include <QWizard>

class QLineEdit;
class QLabel;

class NewFileWizard   : public QWizard
{
	Q_OBJECT
	typedef QWizard super;
public:
    NewFileWizard(QWidget *parent);
    virtual ~NewFileWizard();

private:
	/// welcome page
	QWizardPage *createWelcomePage() const;
	/// destination for a file
	QWizardPage *createFileSearchPage() const;
	/// home church data
	QWizardPage* createHomeChurchPage() const;
	/// main priest data
	QWizardPage* createPriestPage() const;
	/// first client data
	QWizardPage* createClientPage() const;
	/// first holy service input
	QWizardPage* createServicePage() const;
};

///\class ServiceFilePage
/// Wizard page for entering of data file locaion for a new file
/// It needs to be MOC-ed bacuse of utilization of the slot
class ServiceFilePage : public QWizardPage
{
	Q_OBJECT
	typedef QWizardPage super;
public:
    ServiceFilePage(QWidget* = nullptr);
    virtual ~ServiceFilePage();

	/// returns true if m_path and m_filename form a filepath that can be written in
	virtual bool isComplete() const;
	virtual void initializePage();

private slots:
	void browseForFolder();
	/// brings any change in edits into resulting path
	void displayResult();

private:
	QLineEdit *m_path;
	QLineEdit *m_fileName;
	QLabel* m_fileMessageLabel;

	/// isComplete() makes a note about reason to refuse a filename
	mutable QString m_fileMessage;

	QString currentFileName() const;
};

///\class  ChurchPage
/// Auxiliar page for definition of the first church
/// a holy service can be assigned to
class ChurchPage : public QWizardPage
{
	Q_OBJECT
	typedef QWizardPage super;
public:
    ChurchPage(QWidget* = nullptr);
    virtual ~ChurchPage();

	virtual void initializePage();
};

///\class  PriestPage
/// Auxiliar page for definition of the first church
/// a holy service can be assigned to
/// It does not need to be MOC-ed so can be local to a cpp file
class PriestPage : public QWizardPage
{
	Q_OBJECT
	typedef QWizardPage super;
public:
    PriestPage(QWidget* = nullptr);
    virtual ~PriestPage();

	virtual void initializePage();
};

///\class  ClientPage
/// Auxiliar page for definition of some client of holy service
/// a holy service can be assigned to
/// It does not need to be MOC-ed so can be local to a cpp file
class ClientPage : public QWizardPage
{
	Q_OBJECT
	typedef QWizardPage super;
public:
    ClientPage(QWidget* = nullptr);
    virtual ~ClientPage();

	virtual void initializePage();
};

///\class  FirstServicePage
/// Auxiliar page for definition of plain first holy service
/// It does not need to be MOC-ed so can be local to a cpp file
class FirstServicePage : public QWizardPage
{
	Q_OBJECT
	typedef QWizardPage super;
public:
    FirstServicePage(QWidget* = nullptr);
    virtual ~FirstServicePage();

	virtual void initializePage();
};

#endif
