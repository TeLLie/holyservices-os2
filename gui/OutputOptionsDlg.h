#ifndef OUTPUTOPTIONSDLG_H_INCLUDED_2992B1E2_C5FB_4237_9E36_53ADF25732F8
#define OUTPUTOPTIONSDLG_H_INCLUDED_2992B1E2_C5FB_4237_9E36_53ADF25732F8

///\class OutputOptionsDlg
/// Dialog for selection of output format and time range of exported 
/// holy services
///\date 11-2006
///\author Jan 'Kovis' Struhar

#include <QDialog>
#include <QDate>
#include <QMap>

class Ui_OutputOptionsDlg;
class QAbstractButton;
class IHolidays;

class OutputOptionsDlg : public QDialog
{
	Q_OBJECT
	typedef QDialog super;
public:
	enum EOutputFormat { UnknownFormat, XmlFormat, CsvFormat, 
        NativePrinterFormat, PdfFormat, HtmlFormat,
        iCalFormat, OfficeClipboardFormat, OpenOfficeWriterFormat };
    enum EOutputContents { JustServices, IncludeBlankDates };

    OutputOptionsDlg(QWidget* parent);
    virtual ~OutputOptionsDlg();

	void setYear(int year);
    void get(QDate& fromDate, QDate& toDate, QString& clientIdString, 
        EOutputFormat& format, EOutputContents& contents);

    void setHolidays(const IHolidays* holidayProvider);

private slots:
	void setStartingDateToToday();
    void setDateRangeToNextWeek();
	void columnConfiguration();
    void onOk();

private:
	static QDate lastFrom;
	static QDate lastTo;
    static EOutputFormat lastFormat;
    static EOutputContents lastContents;

    Ui_OutputOptionsDlg* m_ui;
    QMap<EOutputFormat, QAbstractButton*> m_formatButtonMap;
    const IHolidays* m_holidays;

    EOutputFormat selectedFormat() const;
};

#endif
