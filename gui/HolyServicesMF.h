#pragma once

///\class CHolyServicesMF
/// Main window of the application holding menu, toolbar
/// and central widget
///\author Jan 'Kovis' Struhar
///\date 6-2006
#include "ui_HolyServicesMF.h"
#include <QMainWindow>
#include <QPointer>
#include <QDateTime>

class IPersistentStore;
class ServiceCalendarWidget;
class QSpinBox;
class QComboBox;
class QSignalMapper;
class QUndoStack;

class HolyServicesMF :	public QMainWindow, Ui::HolyServicesMF
{
	Q_OBJECT
	typedef QMainWindow super;
public:
    HolyServicesMF(void);
    virtual ~HolyServicesMF(void);

public slots:
	void newFile();

protected:
    virtual void closeEvent(QCloseEvent*) override;

private slots:
	void openFile();
    void openParticularFile(const QString& fileName);
	void saveAll();
	
	void clientManagement();
	void priestManagement();
	void churchManagement();
	void churchEditor();

	void preferences();
	void serviceGenerator();
    void shiftServicesInTime();

	void changeYear(int newYear);

	/// magnificient about box
	void about();
	void helpIndex();

	void print();

	/// connected to both combos - changes filtering
    void priestComboChanged(int index); 
    void churchComboChanged(int index); 

	/// join most recently used files
	void addMru();

    void on_actionDearchive_triggered();
    void holidayManagement();
    void countMoneyContributions();

    void on_actionGoogle_Calendar_triggered();

    void assignedWhenClientIsAssigned();
    void assignedWhenIntentionIsFilled();

    void on_actionImport_CSV_triggered();

    void mainViewAsYear();
    void mainViewAsTrimesters();

private:
	/// the main widget with calendar
	ServiceCalendarWidget *m_central;
	/// toolbar placed spinbox that defines holy service filter by year
    QSpinBox* m_yearSpin = nullptr;
	/// toolbar placed combo that defines holy service filter by priest
    QComboBox *m_priestCombo = nullptr;
	/// toolbar placed combo that defines holy service filter by church
    QComboBox *m_churchCombo = nullptr;
    /// disable the combo change handler when church/priest combos are filled
    /// otherwise the UserSettings values get reset to "All/All"
    bool m_toolbarCombosBeingFilled = false;
	/// floating independent window with program documentation
	QPointer<QWidget> m_helpWindow;
	/// disabled when no file is opened, active otherwise
	QList<QAction*> m_actionsActiveOnOpenedFile;
	/// widgets that should be de/activated when file is closed/opened
	QWidgetList m_widgetsActiveOnOpenedFile;

	//@{
	///\name Most recently used files
    QAction *m_mruFilesAction = nullptr;
    QSignalMapper *m_mruMapper = nullptr;
	//@}

    /// undo stack for generator runs - it is kept out of ServiceGeneratorDlg
    /// and reset as new project is opened
    QUndoStack* m_generatorUndo = nullptr;

    //@{
    ///\name GoogleCal token & validity
    QString m_accessToken;
    QDateTime m_accessTokenValidTo;
    //@}

    QActionGroup* m_assignedCriteriaGroup = nullptr;
    QActionGroup* m_mainViewGroup = nullptr;

	/// add combo boxes to toolbar
	void equipToolbar();

	int displayedYear(void) const;

	/// refill priest combo with list of available priests
	void fillPriestCombo();
	/// refill priest combo with list of available churches
	void fillChurchCombo();

	/// method assumes that file is actually already opened, every
	/// data is loaded into appropriate memory indexing structure,
	/// method only sets up UI stuff and notices project to MRU list
	void openFileInternal();

	/// releases all in-memory containers, closes and releases persistent store correctly
	void closeFile();

	/// (de) activate controls dependent on opened file
	void activateFileRelatedStuff(bool enable);

	/// called on startup - gives the filename that is open by default - 
	/// either one given on command line parameter or the last one in recent files list
	/// or the default from version 1.0
	QString defaultFileName();
};
