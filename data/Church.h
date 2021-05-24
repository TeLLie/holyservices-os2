#ifndef CHURCH_H_INCLUDED_2FF1D6B7_C516_4B1D_A927_554BAFD5B79C
#define CHURCH_H_INCLUDED_2FF1D6B7_C516_4B1D_A927_554BAFD5B79C

///\class Church
/// Holds data for building where the holy service will be served
/// along with default color for displaying and service presets
///\date 10-2006
///\author Jan 'Kovis' Struhar

#include "BasicModificationAware.h"
#include "Identifiable.h"
#include <QColor>
#include <QVector>
#include <QTime>

class Church : public BasicModificationAware, public IIdentifiable
{
public:
    Church();
    virtual ~Church();

    virtual const IdTag& getId() const;
    virtual void setId(const IdTag&);

	const QString& name() const;
	void setName(const QString& aName);

    const QString& abbreviation() const;
    void setAbbreviation(const QString& newAbbreviation);

	const QString& place() const;
	void setPlace(const QString& aPlace);

	virtual QString toString() const;

	///\struct ServiceTemplate
	/// List of such records can be defined with church
	/// to serve as template for service generator
	struct ServiceTemplate
	{
        enum ServiceTerm { Monday = 1, Tuesday, Wednesday, Thursday, Friday, Saturday,
                           Sunday1, Sunday2, Sunday3, LastTerm = Sunday3 };

		ServiceTemplate();

		bool m_active;
		IdTag m_priest;
        ServiceTerm m_serviceTerm = Monday;
		QTime m_start;
        int m_durationInMinutes = 45;

		bool operator==(const ServiceTemplate& rhs) const;
                Qt::DayOfWeek weekDay() const;
	};
    typedef QVector<ServiceTemplate> tServiceTemplateList;

	const tServiceTemplateList& usualServices() const;
	void setUsualServices(const tServiceTemplateList&);
	int countOfActiveServices() const;
    static Qt::DayOfWeek equivalent(ServiceTemplate::ServiceTerm);

	QColor color() const;
	void setColor(const QColor&);

    void setEasting(double);
    double easting() const;

    void setNorthing(double);
    double northing() const;

private:
	QString m_name;
    QString m_abbreviation;
	QString m_place;
    double m_easting;
    double m_northing;
	QColor m_color; //< disambiguation color in UI, not building color
	tServiceTemplateList m_usualServices;
    Identifiable m_myIdHandler;
};

#endif
