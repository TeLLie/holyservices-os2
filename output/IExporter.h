#ifndef IEXPORTER_H_INCLUDED_F12F71EF_8035_4E5D_A020_88381CEA538D
#define IEXPORTER_H_INCLUDED_F12F71EF_8035_4E5D_A020_88381CEA538D

///\class IExporter
///\brief Common interface for exporting of holy services into a file
///\date 11-2006
///\author Jan 'Kovis' Struhar 

class HolyService;
class IHolidays;
class QString;
class QDate;

class IExporter
{
public:
    virtual ~IExporter();

	/// initializes export, usually opens file, sets exported column list
	/// and writes file header
	///\return true if initialization was successful
	virtual bool init(const QString& fileName, const QString& options) = 0;
    /// export one record
    virtual void write(const HolyService& hs, int options = 0) = 0;
    /// export one blank record
    virtual void writeBlankLine(const QDate& dt, int options = 0);
    /// write export footer (if needed) and close export file
	virtual void close() = 0;	

    void setHolidayProvider(const IHolidays*);

protected:
    IExporter();

    const IHolidays* holidayProvider() const;

private:
    /// not owned reference to a holiday provider
    const IHolidays* m_holidayProvider;
};

#endif

