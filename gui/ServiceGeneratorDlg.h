#ifndef SERVICEGENERATORDLG_H_INCLUDED_E0690905_D040_45B5_A697_50E4779DBC40
#define SERVICEGENERATORDLG_H_INCLUDED_E0690905_D040_45B5_A697_50E4779DBC40

///\class ServiceGeneratorDlg
///\brief Dialog for generating of holy services
///\date 10-2006
///\author Jan 'Kovis' Struhar
#include "IdComboBox.h"
#include "HsTypedefs.h"
#include <QDialog>

class QAbstractItemModel;
class QUndoStack;
class Ui_ServiceGeneratorDlg;

class ServiceGeneratorDlg : public QDialog
{
	Q_OBJECT
	typedef QDialog super;
public:
    explicit ServiceGeneratorDlg(QWidget* parent, const int year);
    virtual ~ServiceGeneratorDlg();

    /// it is assumed that undo is assigned before exec() - if it is set at all
    void setUndo(QUndoStack*);

private slots:
	void generate();
	void setStableControlsEnabled(bool);
    void undoRun();
    void redoRun();

private:
	enum ECollisionBehavior { UndefinedBehavior, SkipCollision, ReplaceCollision };

    Ui_ServiceGeneratorDlg* m_ui;

	QAbstractItemModel* m_clientModel;
	QAbstractItemModel* m_churchModel;
	QAbstractItemModel* m_priestModel;

    /// not owned undo information from main window
    QUndoStack* m_undo;

	void transferToAllSet(const tHolyServiceVector& services, 
		const ECollisionBehavior behavior);
	/// check for suspicious generator input
	bool checkSuspiciousness(QString& validationMsg) const;

	/// check validity
	bool checkValidity(QString& validationMsg) const;

    void updateUndoButtons();
};

#endif
