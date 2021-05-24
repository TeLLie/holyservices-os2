#ifndef CLIENTOVERVIEW_H_INCLUDED_F73B4F0B_0569_429E_B042_EB1301C7A1BD
#define CLIENTOVERVIEW_H_INCLUDED_F73B4F0B_0569_429E_B042_EB1301C7A1BD

///\class ClientOverview
/// Single tables management window - list through items, edit, add, delete them
/// Despite the name it edits all definitions - clients, priests, churches, for they
/// are very similar by nature
///\date 10-2006
///\author Jan 'Kovis' Struhar

#include "HsTypedefs.h"
#include <QDialog>
#include <QAbstractItemDelegate>
#include <QAbstractButton>

class Client;
class Ui_ClientOverview;

class ClientOverview : public QDialog
{
	Q_OBJECT
    typedef QDialog super;
public:
	enum EEditedObject { UNKNOWN, CLIENT, CHURCH, PRIEST };
    ClientOverview(QWidget *parent, const EEditedObject editedType);
    virtual ~ClientOverview();

private slots:
	void onDeleteButton();
	void onAddButton();

	/// model is re-sorted after editation of key - it is nice to continue editation
	void handleMoveOnKeyChange(QWidget*, QAbstractItemDelegate::EndEditHint);
	void noticeKeyIndexChange(const QModelIndex&);

private:
	static const int keyColumn;

    Ui_ClientOverview* m_ui;
	const EEditedObject m_editedType;
	QAbstractItemModel* m_model;
	QModelIndex m_lastChangedKeyIndex;

	QAbstractButton *m_deleteButton;
	QAbstractButton *m_addButton;

	void initializeTableColumns();
	void initializeWindowTitle();
	void setupButtons();

	void deleteClient(const QModelIndex&);
	void deletePriest(const QModelIndex&);
	void deleteChurch(const QModelIndex&);

	///{@
	/// Message about how many holy services refer given item
    QString dependentMsg(Client *);
    QString dependentMsg(Priest *);
    QString dependentMsg(Church *);
	///@}
};

// enable Queued connection
Q_DECLARE_METATYPE(QModelIndex);

#endif
