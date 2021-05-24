#ifndef CHURCHWEEKDAYWIDGET_H_INCLUDED_D1C11E81_C8B7_45B4_A625_581803606F7A
#define CHURCHWEEKDAYWIDGET_H_INCLUDED_D1C11E81_C8B7_45B4_A625_581803606F7A

///\class ChurchWeekDayWidget
/// Widget displaying service template record for certain day of week
///\date 12-2009
///\author Jan 'Kovis' Struhar
#include <QWidget>

#include "Church.h"

class QCheckBox;
class QTimeEdit;
class QSpinBox;
class IdComboBox;

class ChurchWeekDayWidget : public QWidget
{
    Q_OBJECT
    typedef QWidget super;
public:
    ChurchWeekDayWidget(QWidget* p);
    virtual ~ChurchWeekDayWidget();

    void set(const Church::ServiceTemplate& data);
    void get(Church::ServiceTemplate& data) const;

    // from left to right the set maximal widths for the widgets
    static QList<int> maximalWidths();

private slots:
    /// disable rest of widgets when day is checked off
    void weekDayChecked();

private:
    QString weekDayText() const;

    QCheckBox* m_weekDayLabel;
    IdComboBox* m_priestCombo;
    QTimeEdit* m_startTimeEdit;
    QSpinBox* m_durationBox;
    Church::ServiceTemplate::ServiceTerm m_serviceTerm;
};

#endif
