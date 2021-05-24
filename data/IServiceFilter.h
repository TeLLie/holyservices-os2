#ifndef ISERVICE_FILTER_H_INCLUDED_6DBE570E_ED54_4634_961B_5A75C51307D6
#define ISERVICE_FILTER_H_INCLUDED_6DBE570E_ED54_4634_961B_5A75C51307D6

class HolyService;

///\class IServiceFilter
/// Generic interface for filtering of services - descendants will probably 
/// have some filter criteria settings like - restrict on particular church, priest a.s.o.
///\date 9-2006
///\author Jan 'Kovis' Struhar kovis@sourceforge.net
class IServiceFilter
{
	public:
		virtual ~IServiceFilter() {};

		virtual bool pass(const HolyService* const holyService) const = 0;
};

#endif

