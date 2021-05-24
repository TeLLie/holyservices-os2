#ifndef CHURCHWEEKTEMPLATEWIDGET_H
#define CHURCHWEEKTEMPLATEWIDGET_H

#include "Church.h"
#include <QWidget>

class ChurchWeekDayWidget;

class ChurchWeekTemplateWidget : public QWidget
{
    Q_OBJECT
    typedef QWidget super;
public:
    explicit ChurchWeekTemplateWidget(QWidget *parent = nullptr);
    virtual ~ChurchWeekTemplateWidget();

    void set(const Church::tServiceTemplateList& data);
    void get(Church::tServiceTemplateList& data) const;

private:
    QList<ChurchWeekDayWidget*> m_weekDayWidgets;

};

#endif // CHURCHWEEKTEMPLATEWIDGET_H
