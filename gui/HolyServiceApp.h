#ifndef HOLYSERVICEAPP_H_INCLUDED_5229A2A1_F44A_43FD_90DE_3F0948387CD9
#define HOLYSERVICEAPP_H_INCLUDED_5229A2A1_F44A_43FD_90DE_3F0948387CD9

///\class HolyServiceApp
///\bried exception catching application object
/// Overriding notify() in order to be able exceptions thrown
/// by persistence engine
///
///\date 3-2007
///\author Jan 'Kovis' Struhar
#include <QApplication>

class HolyServiceApp : public QApplication
{
	Q_OBJECT
	typedef QApplication super;
public:
    HolyServiceApp(int& argc, char** argv);
    virtual ~HolyServiceApp();

	virtual bool notify(QObject *receiver, QEvent *event);
};

#endif

