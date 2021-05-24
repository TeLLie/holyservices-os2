#include "UpdateGuard.h"

#include <QWidget>

UpdateGuard::UpdateGuard(QWidget* aWidget) : m_widget(aWidget)
{
	if (m_widget)
	{
		m_widget->setUpdatesEnabled(false);
	}
}

UpdateGuard::~UpdateGuard()
{
	if (m_widget)
	{
		m_widget->setUpdatesEnabled(true);
	}
}

