#pragma once
#include "../data/IHistoryData.h"

///\class HistoryData
///\brief Common implementation of IHistoryData interface
///\author Jan 'Kovis' Struhar
///\date 9/2006

class HistoryData : public IHistoryData
{
public:
    HistoryData(void);
    virtual ~HistoryData(void);

	virtual QDateTime created() const;
	virtual void setCreated(const QDateTime&);

	virtual QString creator() const;
	virtual void setCreator(const QString&);

	virtual QDateTime lastModified() const;
	virtual void setLastModified(const QDateTime&);

	virtual QString lastModificator() const;
	virtual void setLastModificator(const QString&);

	virtual void modifyNow();
	virtual void createNow();

private:
	/// returns string about current user@computer
	static QString currentUserAndComputer();
	static QString currentUser();
	static QString currentComputer();

	QString m_creator;
	QDateTime m_created;
	QString m_lastModificator;
	QDateTime m_lastModified;
};
