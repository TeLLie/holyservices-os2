#ifndef CALENDARDELEGATE_H_INCLUDED_3D74DB26_18C1_4004_849E_27B88CBF7617
#define CALENDARDELEGATE_H_INCLUDED_3D74DB26_18C1_4004_849E_27B88CBF7617

///\class CalendarDelegate
///\brief Draws striped colors if more churches have color in the same day
///\date 9-2007
///\author Jan 'Kovis' Struhar
#include <QItemDelegate>

class CalendarDelegate : public QItemDelegate
{
	Q_OBJECT
	typedef QItemDelegate super;
public:
    CalendarDelegate(QObject* = nullptr);
    virtual ~CalendarDelegate();

	virtual void paint ( QPainter * painter, const QStyleOptionViewItem & option, 
		const QModelIndex & index ) const;
private:

};

#endif
