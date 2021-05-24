#include "ChurchWeekTemplateWidget.h"

#include "ChurchWeekDayWidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QSet>

ChurchWeekTemplateWidget::ChurchWeekTemplateWidget(QWidget* p) : super(p)
{
        const QList<int> maxWidths = ChurchWeekDayWidget::maximalWidths();
        QGridLayout* weekLayout = new QGridLayout(this);
        QLabel* aLabel;

        weekLayout->setMargin(0);

        aLabel = new QLabel(tr("Weekday"), this);
        weekLayout->addWidget(aLabel, 0, 0);

        aLabel = new QLabel(tr("Priest"), this); aLabel->setMaximumWidth(maxWidths.at(1));
        weekLayout->addWidget(aLabel, 0, 1);

        aLabel = new QLabel(tr("Begin at", "start at certain time"), this); aLabel->setMaximumWidth(maxWidths.at(2));
        weekLayout->addWidget(aLabel, 0, 2);

        aLabel = new QLabel(tr("Duration"), this); aLabel->setMaximumWidth(maxWidths.at(3));
        weekLayout->addWidget(aLabel, 0, 3);

        // 7 days in a week
        for (int day = Church::ServiceTemplate::Monday; day <= Church::ServiceTemplate::LastTerm; ++day)
        {
                m_weekDayWidgets << new ChurchWeekDayWidget(this);
                weekLayout->addWidget(m_weekDayWidgets.last(), day, 0, 1, 4);
        }
}

ChurchWeekTemplateWidget::~ChurchWeekTemplateWidget()
{
}

void ChurchWeekTemplateWidget::set( const Church::tServiceTemplateList& data )
{
        // countermeasurement against multiple entries of one weekday
        QSet<int> alreadyFilled;

        for (int term = Church::ServiceTemplate::Monday; term <= Church::ServiceTemplate::LastTerm; ++term )
        {
                Church::ServiceTemplate dayTemplate;

                dayTemplate.m_serviceTerm = static_cast<Church::ServiceTemplate::ServiceTerm>(term);
                dayTemplate.m_active = false;

                // find first template for day
                foreach(const Church::ServiceTemplate& p, data)
                {
                        if (p.m_serviceTerm == term)
                        {
                                dayTemplate = p;
                        }
                }

                ChurchWeekDayWidget* pDayWidget = m_weekDayWidgets.at(term - 1);
                if (pDayWidget)
                {
                        pDayWidget->set(dayTemplate);
                }
        }
}

void ChurchWeekTemplateWidget::get( Church::tServiceTemplateList& data ) const
{
        data.clear();

        for (int index = 0; index < m_weekDayWidgets.count(); ++index)
        {
                ChurchWeekDayWidget* w = m_weekDayWidgets.at(index);
                Church::ServiceTemplate oneDayTemplate;

                w->get(oneDayTemplate);

                data.append(oneDayTemplate);
        }
}
