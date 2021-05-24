/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ChurchWeekDayWidget.h"

#include "PriestIndexing.h"
#include "PriestComboBox.h"

#include <QCheckBox>
#include <QTimeEdit>
#include <QSpinBox>
#include <QHBoxLayout>

ChurchWeekDayWidget::ChurchWeekDayWidget(QWidget* p) : super(p)
{
    new QHBoxLayout(this);
	layout()->setMargin(0);

    m_weekDayLabel = new QCheckBox(this);
    connect(m_weekDayLabel, SIGNAL(stateChanged(int)), SLOT(weekDayChecked()));
    layout()->addWidget(m_weekDayLabel);

    m_priestCombo = new CPriestComboBox(this);
    m_priestCombo->setMaximumWidth(maximalWidths().at(1));
    layout()->addWidget(m_priestCombo);

    m_startTimeEdit = new QTimeEdit(this);
    m_startTimeEdit->setMaximumWidth(maximalWidths().at(2));
    m_startTimeEdit->setAlignment(Qt::AlignRight);
	m_startTimeEdit->setDisplayFormat("hh:mm");
    layout()->addWidget(m_startTimeEdit);

    m_durationBox = new QSpinBox(this);
    m_durationBox->setMaximumWidth(maximalWidths().at(2));
    m_durationBox->setAlignment(Qt::AlignRight);
    m_durationBox->setMinimum(10);
    m_durationBox->setMaximum(360);
    m_durationBox->setSingleStep(10);
    m_durationBox->setSuffix(' ' + tr("min", "minutes abbreviation"));
    layout()->addWidget(m_durationBox);

        m_serviceTerm = Church::ServiceTemplate::Monday;

    // disable widget initially
    m_weekDayLabel->setChecked(false);
    weekDayChecked();
}

ChurchWeekDayWidget::~ChurchWeekDayWidget()
{
}

QList<int> ChurchWeekDayWidget::maximalWidths()
{
    static QList<int> widgetMaximalWidths;

    if (widgetMaximalWidths.isEmpty())
    {
        widgetMaximalWidths << -1 << 250 << 110 << 110;
    }
    return widgetMaximalWidths;
}

void ChurchWeekDayWidget::set( const Church::ServiceTemplate& data )
{
        m_serviceTerm = data.m_serviceTerm;
    m_weekDayLabel->setText(weekDayText());
    m_weekDayLabel->setChecked(data.m_active);
    m_startTimeEdit->setTime(data.m_start);
    m_durationBox->setValue(data.m_durationInMinutes);
	m_priestCombo->setCurrentId(data.m_priest);
}

void ChurchWeekDayWidget::get( Church::ServiceTemplate& data ) const
{
	data.m_active = m_weekDayLabel->isChecked();
	data.m_priest = m_priestCombo->currentId();
	data.m_start = m_startTimeEdit->time();
	data.m_durationInMinutes = m_durationBox->value();
        data.m_serviceTerm = m_serviceTerm;
}

void ChurchWeekDayWidget::weekDayChecked()
{
    const QWidgetList rowWidgets = QWidgetList() <<
              m_priestCombo << m_startTimeEdit << m_durationBox;
    const bool active = m_weekDayLabel->isChecked();

    foreach(QWidget* w, rowWidgets)
    {
        w->setEnabled(active);
    }
}

QString ChurchWeekDayWidget::weekDayText() const
{
        switch (m_serviceTerm)
        {
        case Church::ServiceTemplate::Monday: return tr("Monday");
        case Church::ServiceTemplate::Tuesday: return tr("Tuesday");
        case Church::ServiceTemplate::Wednesday: return tr("Wednesday");
        case Church::ServiceTemplate::Thursday: return tr("Thursday");
        case Church::ServiceTemplate::Friday: return tr("Friday");
        case Church::ServiceTemplate::Saturday: return tr("Saturday");
        case Church::ServiceTemplate::Sunday1: return tr("Sunday 1");
        case Church::ServiceTemplate::Sunday2: return tr("Sunday 2");
        case Church::ServiceTemplate::Sunday3: return tr("Sunday 3");
        }
	return "???";
}

