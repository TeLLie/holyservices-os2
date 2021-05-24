#ifndef CHURCHEDITOR_H_INCLUDED_15FA9C05_5A3F_493C_A583_154E1A545300
#define CHURCHEDITOR_H_INCLUDED_15FA9C05_5A3F_493C_A583_154E1A545300

///\class ChurchEditor
/// Church data editor
///\date 12-2009
///\author Jan 'Kovis' Struhar
#include <QDialog>
#include <QModelIndex>
#include "Church.h"

class Ui_ChurchEditor;
class ChurchWeekDayWidget;
class ChurchWeekTemplateWidget;

class ChurchEditor : public QDialog
{
	Q_OBJECT
	typedef QDialog super;
public:
    ChurchEditor(QWidget* p);
    virtual ~ChurchEditor();

private slots:
	void handleRowChanged(const QModelIndex& currentRow);
	void handleChurchColorSelection();
	void onOkButton();

private:
	Ui_ChurchEditor* m_ui;
	Church* m_edited;
	QColor m_churchColor; ///< needs to be kept separately
        ChurchWeekTemplateWidget* m_weekWidget;

	void objectToDialog();
	void dialogToObject();
	void indicateChurchColorOnColorButton();
};

#endif
