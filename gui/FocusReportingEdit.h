#ifndef FOCUSREPORTINGEDIT_H_INCLUDED_2975D1E8_9FAB_49C8_90F9_35FF425E1758
#define FOCUSREPORTINGEDIT_H_INCLUDED_2975D1E8_9FAB_49C8_90F9_35FF425E1758

///\class FocusReportingEdit
///\brief Class reports focus acquisition by signal emitting
///\date 11-2006
///\author Jan 'Kovis' Struhar

#include <QTextEdit>

class FocusReportingEdit : public QTextEdit
{
	Q_OBJECT
	typedef QTextEdit super;
public:
    FocusReportingEdit(QWidget* parent = nullptr);
    virtual ~FocusReportingEdit();

signals:
	void acquiredFocus();

protected:
	virtual void focusInEvent(QFocusEvent *);
};

#endif
