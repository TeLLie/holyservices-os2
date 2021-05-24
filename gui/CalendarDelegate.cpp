/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "CalendarDelegate.h"

#include <QPainter>
#include <QModelIndex>
#include <QtDebug>

CalendarDelegate::CalendarDelegate(QObject *p) : super(p)
{
}

CalendarDelegate::~CalendarDelegate()
{
}

void CalendarDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option,
		const QModelIndex & index ) const
{
	super::paint(painter, option, index);

	if (option.state & QStyle::State_Selected)
	{
		QVariant cellData = index.data(Qt::BackgroundRole);

		if (cellData.isValid() && cellData.canConvert<QBrush>())
		{
			static const double stripWidth = 7.0;
			const QBrush cellBrush = cellData.value<QBrush>();
			
			//qDebug() << "Cell brush " << cellBrush << "Data type " << cellData.type();

			painter->setPen(QPen(cellBrush, stripWidth, Qt::SolidLine, Qt::FlatCap));
			QPolygonF strip;

			strip << option.rect.topLeft() << option.rect.bottomLeft();

			strip.translate(stripWidth / 2.0, 0.0);
			painter->drawPolygon(strip);
			//painter->setBrush(cellBrush);
			//painter->drawRect(option.rect);
		}
	}
}

