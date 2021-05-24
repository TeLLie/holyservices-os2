#ifndef DATEINPUTDLG_H_INCLUDED_7194C657_A81C_4CA1_A62E_909A6D4BA3F0
#define DATEINPUTDLG_H_INCLUDED_7194C657_A81C_4CA1_A62E_909A6D4BA3F0

///\class DateInputDlg
///\brief Simple dialog for date input
///\date 11-2006
///\author Jan 'Kovis' Struhar
#include <QDate>
#include <QDialog>

class Ui_DateInputDlg;

class DateInputDlg : public QDialog
{
	typedef QDialog super;
public:
    DateInputDlg(QWidget*);
    virtual ~DateInputDlg();

	void setDate(const QDate&);
	QDate getDate() const;

private:
    Ui_DateInputDlg* m_ui;
};

#endif
