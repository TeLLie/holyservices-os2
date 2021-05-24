#ifdef ICELEBRATIONS_H_INCLUDED_D005A1FB_67DE_4E21_99C0_DBE1D3066D23
#define ICELEBRATIONS_H_INCLUDED_D005A1FB_67DE_4E21_99C0_DBE1D3066D23

///\class ICelebrations
/// Class gives information about significant celebration day throughout a year
/// - especially important is moving date of Eastern for given year
///\date 9-2006
///\author Jan 'Kovis' Struhar

class ICelebrations
{
	public:
		virtual ~ICelebrations() {}

		virtual bool isCelebratingDay(const QDate&, QString *description) = 0;
};

#endif
