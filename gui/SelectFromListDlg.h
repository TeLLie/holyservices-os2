#ifndef SELECTFROMLISTDLG_H_INCLUDED_5034E10B_9811_4C63_8CA9_F54F01E81228
#define SELECTFROMLISTDLG_H_INCLUDED_5034E10B_9811_4C63_8CA9_F54F01E81228

///\class SelectFromListDlg
///\brief Generic selection of an item from list of strings
///\date 10-2006
///\author Jan 'Kovis' Struhar
#include <QDialog>

class QStringList;
class QModelIndex;
class Ui_SelectFromListDlg;

class SelectFromListDlg : public QDialog
{
	Q_OBJECT
	typedef QDialog super;
public:
    SelectFromListDlg(QWidget *parent, const QStringList& list);
    virtual ~SelectFromListDlg();

    int currentItemIndex() const;

private slots:
	void listDoubleClicked ( const QModelIndex & index );

private:
    Ui_SelectFromListDlg* m_ui;
};

#endif

