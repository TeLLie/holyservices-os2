/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ChurchEditor.h"

#include "ui_ChurchEditor.h"
#include "ChurchWeekDayWidget.h"
#include "ChurchWeekTemplateWidget.h"
#include "ButtonDecorator.h"
#include "ChurchIndexing.h"
#include "ChurchModel.h"

#include <QColorDialog>

ChurchEditor::ChurchEditor(QWidget* p) : super(p)
{
	m_ui = new Ui_ChurchEditor;
	m_ui->setupUi(this);

	m_edited = nullptr;
	ButtonDecorator::assignIcons(*m_ui->buttonBox);

	connect(m_ui->churchColorBtn, SIGNAL(clicked()), SLOT(handleChurchColorSelection()));

	// populate the table
	m_ui->churchTable->setModel(new ChurchModel(this));
	m_ui->churchTable->resizeColumnsToContents();
	connect(m_ui->churchTable->selectionModel(), 
		SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)),
		SLOT(handleRowChanged(const QModelIndex&)));
	connect(m_ui->buttonBox, SIGNAL(accepted()), SLOT(onOkButton()));

        m_weekWidget = new ChurchWeekTemplateWidget(this);

	// finally insert the filled grid into main vertical box layout
	QBoxLayout* mainLayout = dynamic_cast<QBoxLayout*>(layout());
	Q_ASSERT(mainLayout);
	if (mainLayout)
	{
		mainLayout->insertWidget(2, m_weekWidget);
	}
}

ChurchEditor::~ChurchEditor()
{
	delete m_ui;
}

void ChurchEditor::handleRowChanged( const QModelIndex& currentRow )
{
	if (! currentRow.isValid()) return;

	// save edited item
	dialogToObject();

	const IdTag churchId = IdTag::createFromString(currentRow.data(Qt::UserRole).toString());
	m_edited = ChurchIndexing::allSet().find(churchId);

	objectToDialog();
}

void ChurchEditor::objectToDialog()
{
	if (m_edited == nullptr) return;

	m_ui->churchDescriptionEdit->setText(m_edited->name());
	m_ui->churchPlaceEdit->setText(m_edited->place());

	m_churchColor = m_edited->color();
	indicateChurchColorOnColorButton();

	m_weekWidget->set(m_edited->usualServices());
}

void ChurchEditor::dialogToObject()
{
	if (m_edited == nullptr) return;

	m_edited->setName(m_ui->churchDescriptionEdit->text());
	m_edited->setPlace(m_ui->churchPlaceEdit->text());

	m_edited->setColor(m_churchColor);

	Church::tServiceTemplateList serviceTemplates;
	m_weekWidget->get(serviceTemplates);
	m_edited->setUsualServices(serviceTemplates);
}

void ChurchEditor::indicateChurchColorOnColorButton()
{
	QPixmap pix(16, 16);

	pix.fill(m_churchColor);
	m_ui->churchColorBtn->setIcon(QIcon(pix));
}

void ChurchEditor::handleChurchColorSelection()
{
	if (m_edited == nullptr) return;

	const QColor newColor = QColorDialog::getColor(m_edited->color(), this);
	if (newColor.isValid())
	{
		m_churchColor = newColor;
		indicateChurchColorOnColorButton();
	}
}

void ChurchEditor::onOkButton()
{
	dialogToObject();
	accept();
}
