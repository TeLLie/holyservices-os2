/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** Licensees holding a valid Qt License Agreement may use this file in
** accordance with the rights, responsibilities and obligations
** contained therein.  Please consult your licensing agreement or
** contact sales@trolltech.com if any conditions of this licensing
** agreement are not clear to you.
**
** Further information about Qt licensing is available at:
** http://www.trolltech.com/products/qt/licensing.html or by
** contacting info@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
// Slightly adopted as guaranteed by GPLv2

#ifndef CALENDAR_VIEW_H_INCLUDED_6D5AD5F6_F4FF_446A_B518_CE980E3A0D05
#define CALENDAR_VIEW_H_INCLUDED_6D5AD5F6_F4FF_446A_B518_CE980E3A0D05

#include <QTableView>

class CalendarView : public QTableView
{
    Q_OBJECT
public:
    CalendarView(QWidget *parent = nullptr);

    void internalUpdate() { updateGeometries(); }

    bool weekNumbersShown() const;
    void setWeekNumbersShown(bool show);

    bool dayNamesShown() const;
    void setDayNamesShown(bool show);

    bool isReadOnly() const;
    void setReadOnly(bool enable);

signals:
    void changeDate(const QDate &date, bool changeMonth);
    void editingFinished();

protected:
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool event(QEvent *event) override;

private:
    QDate handleMouseEvent(QMouseEvent *event);
    bool m_validDateClicked;
    bool m_weekNumbersShown;
    bool m_dayNamesShown;
    bool m_readOnly;
};

#endif



