#ifndef IHISTORYDATA_INCLUDED_C8FE45B8_E597_4D79_88AE_D98BAB53B5EF
#define IHISTORYDATA_INCLUDED_C8FE45B8_E597_4D79_88AE_D98BAB53B5EF

#include <QString>
#include <QDateTime>

///\class IHistoryData
/// Interface for keeping historical records about creator and
/// last modificator
///\date 9-2006
///\author Jan 'Kovis' Struhar kovis@sourceforge.net
class IHistoryData
{
public:
	virtual QDateTime created() const = 0;
	virtual void setCreated(const QDateTime&) = 0;

	virtual QString creator() const = 0;
	virtual void setCreator(const QString&) = 0;

	virtual QDateTime lastModified() const = 0;
	virtual void setLastModified(const QDateTime&) = 0;

	virtual QString lastModificator() const = 0;
	virtual void setLastModificator(const QString&) = 0;

	//@{
	///\name Convenience functions
	/// set the modification data to current time and current user
	virtual void modifyNow() = 0;
	/// set the creation data to current time and current user
	virtual void createNow() = 0;
	//@}

	virtual ~IHistoryData() {};
};

#endif


