#ifndef ABOUTDLG_H_INCLUDED_2B887C30_8E60_4809_80C3_21E3048BB439
#define ABOUTDLG_H_INCLUDED_2B887C30_8E60_4809_80C3_21E3048BB439

///\class AboutDlg
/// About dialog holding author info, mail list hyperlink, 
/// shows version information
///\date 11-2006
///\author Jan 'Kovis' Struhar

#include <QDialog>

class Ui_AboutDlg;
class AboutDlg : public QDialog
{
	Q_OBJECT
	typedef QDialog super;
public:
    AboutDlg(QWidget* parent);
    virtual ~AboutDlg();

private:
    Ui_AboutDlg* m_ui;
};

#endif
