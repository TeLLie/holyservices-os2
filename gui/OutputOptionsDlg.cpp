/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "OutputOptionsDlg.h"
#include "ui_OutputOptionsDlg.h"

#include "ButtonDecorator.h"
#include "ReportColumnConfigurationDlg.h"
#include "../preferences/UserSettings.h"

#include <QMessageBox>

QDate OutputOptionsDlg::lastFrom;
QDate OutputOptionsDlg::lastTo;
OutputOptionsDlg::EOutputFormat OutputOptionsDlg::lastFormat = OutputOptionsDlg::UnknownFormat;
OutputOptionsDlg::EOutputContents OutputOptionsDlg::lastContents = OutputOptionsDlg::JustServices;

OutputOptionsDlg::OutputOptionsDlg(QWidget* parent) : super(parent), m_holidays(nullptr)
{
    m_ui = new Ui_OutputOptionsDlg;
    m_ui->setupUi(this);

    m_formatButtonMap[CsvFormat] = m_ui->csvButton;
    m_formatButtonMap[XmlFormat] = m_ui->xmlButton;
    m_formatButtonMap[NativePrinterFormat] = m_ui->printerButton;
    m_formatButtonMap[PdfFormat] = m_ui->pdfButton;
    m_formatButtonMap[HtmlFormat] = m_ui->htmlButton;
    m_formatButtonMap[iCalFormat] = m_ui->iCalButton;
    m_formatButtonMap[OfficeClipboardFormat] = m_ui->officeClipboardButton;
    m_formatButtonMap[OpenOfficeWriterFormat] = m_ui->openOfficeButton;

    ButtonDecorator::assignIcons(*m_ui->buttonBox);

    connect(m_ui->todayButton, &QAbstractButton::clicked,
            this, &OutputOptionsDlg::setStartingDateToToday);
    connect(m_ui->nextWeekButton, &QAbstractButton::clicked,
            this, &OutputOptionsDlg::setDateRangeToNextWeek);
    connect(m_ui->columnSelectionButton, &QAbstractButton::clicked,
            this, &OutputOptionsDlg::columnConfiguration);
    connect(m_ui->buttonBox, &QDialogButtonBox::accepted,
            this, &OutputOptionsDlg::onOk);
}

OutputOptionsDlg::~OutputOptionsDlg()
{
    delete m_ui;
}

void OutputOptionsDlg::setYear(int year)
{
    if (lastFrom.isNull())
    {
        m_ui->fromDateEdit->setDate(QDate(year, 1, 1));
        m_ui->toDateEdit->setDate(QDate(year, 12, 31));
    }
    else
    {
        m_ui->fromDateEdit->setDate(lastFrom);
        m_ui->toDateEdit->setDate(lastTo);
    }

    if ((lastFormat != UnknownFormat) && (m_formatButtonMap.contains(lastFormat)))
    {
        m_formatButtonMap[lastFormat]->setChecked(true);
    }
    if (lastContents == JustServices)
    {
        m_ui->justServicesButton->setChecked(true);
    }
    else
    {
        m_ui->includeBlankButton->setChecked(true);
    }
}

OutputOptionsDlg::EOutputFormat OutputOptionsDlg::selectedFormat() const
{
    auto format = UnknownFormat;
    QMapIterator<EOutputFormat, QAbstractButton*> it(m_formatButtonMap);

    while (it.hasNext())
    {
        it.next();
        if (it.value()->isChecked())
        {
            format = it.key();
            break;
        }
    }
    return format;
}

void OutputOptionsDlg::get(QDate& fromDate, QDate& toDate, QString& clientIdString,
                           EOutputFormat& format, EOutputContents &contents)
{
    lastFrom = fromDate = m_ui->fromDateEdit->date();
    lastTo = toDate = m_ui->toDateEdit->date();

    clientIdString = m_ui->clientCombo->currentIdString();

    format = selectedFormat();
    Q_ASSERT(format != UnknownFormat);

    if (format != UnknownFormat)
    {
        lastFormat = format;
    }

    contents = (m_ui->justServicesButton->isChecked()) ? JustServices : IncludeBlankDates;
    lastContents = contents;
}

void OutputOptionsDlg::setHolidays(const IHolidays *holidayProvider)
{
    m_holidays = holidayProvider;
}

void OutputOptionsDlg::setStartingDateToToday()
{
    m_ui->fromDateEdit->setDate(QDate::currentDate());
}

void OutputOptionsDlg::setDateRangeToNextWeek()
{
    QDate startDate = QDate::currentDate();
    while (startDate.dayOfWeek() != Qt::Sunday) startDate = startDate.addDays(1);
    m_ui->fromDateEdit->setDate(startDate);
    m_ui->toDateEdit->setDate(startDate.addDays(7));
}

void OutputOptionsDlg::columnConfiguration()
{
    ReportColumnConfigurationDlg dlg(this);

    dlg.setColumnConfiguration(UserSettings::get().reportColumns());
    dlg.setHolidays(m_holidays);

    if (dlg.exec() == QDialog::Accepted)
    {
        // change the CUserSettings column layout
        UserSettings::get().setReportColumns(dlg.columnConfiguration());
    }
}

void OutputOptionsDlg::onOk()
{
    if (! UserSettings::get().someColumnIsVisible())
    {
        const QSet<OutputOptionsDlg::EOutputFormat> columnDependent({
            OutputOptionsDlg::CsvFormat, OutputOptionsDlg::NativePrinterFormat,
            OutputOptionsDlg::PdfFormat, OutputOptionsDlg::HtmlFormat,
            OutputOptionsDlg::OfficeClipboardFormat, OutputOptionsDlg::OpenOfficeWriterFormat });
        if (columnDependent.contains(selectedFormat()))
        {
            QMessageBox::warning(this, tr("Warning"),
                tr("You have requested report based on column list\nbut no columns are selected.\n\nPlease select some columns to be printed."));
            columnConfiguration();
            return;
        }
    }
    accept();
}

