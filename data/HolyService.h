#pragma once
#include "BasicModificationAware.h"
#include "../common/HistoryData.h"
#include "Identifiable.h"

#include <QString>
#include <QDateTime>
#include <chrono>

class Client;
class Priest;
class Church;

///\class HolyService
/// Central data class of the applications - it binds client, church and priest together
/// within a time frame with an service intention. 
/// Class also keeps a history record - who created the record and who modified it recently.
/// Service can have any length, however length 
/// over several hours is suspicious and UI warns about it.
///\date 8-2006
///\author Jan 'Kovis' Struhar
class HolyService : public IModificationAware, public IIdentifiable, public HistoryData
{
public:
    HolyService(void);
    virtual ~HolyService(void);

    /// in 2011 J. Kotvrda wanted to distinguish some services that are not firmly bound
    /// to a original date - that the client does not mind moving it
    enum TimeConstraint { FixedToDate, Movable };

	void setIntention(const QString& anIntention);
	const QString& intention() const;
    bool hasIntention() const;

	void setStartTime(const QDateTime& aStartingTime);
	const QDateTime& startTime() const;

	void setEndTime(const QDateTime& anEndTime);
	const QDateTime& endTime() const;

    void moveInTime(const std::chrono::minutes& offset);

	/// length of holy service in seconds
	int lengthSeconds() const;

	/// holy service conflicts, if priest, church is the same and time of service overlaps
    bool conflicts(const HolyService& another, QString *report = nullptr) const;

	bool isValid() const;

    enum TextFormat { DefaultFormat, ShortFormat };
    QString toString(TextFormat format = DefaultFormat ) const;

    /// id will stay unchanged but all other data items are swapped
    void swap(HolyService& other);

    /// put complete verbatim copy of this into the other - excluding the identity
    /// could be thought as copy constructor, used for sake of undo
    void putCopyInto(HolyService& other) const;

	//@{
	///\name Holy service client
	bool hasClient() const;
    Client* getClient() const;
	void setClientId(const IdTag& clientId);
    void setClient(const Client* const pClient);
	QString clientText() const;
	//@}

	//@{
	///\name Holy service church
	bool hasChurch() const;
    Church* getChurch() const;
	void setChurchId(const IdTag& churchId);
    void setChurch(const Church* const pChurch);
	QString churchText() const;
	//@}

	//@{
	///\name Priest
	bool hasPriest() const;
    Priest* getPriest() const;
	void setPriestId(const IdTag& priestId);
    void setPriest(const Priest* const pPriest);
	QString priestText() const;
	//@}

    TimeConstraint timeConstraint() const;
    void setTimeConstraint(TimeConstraint constraint);

    double receivedMoney() const;
    void setReceivedMoney(double newValue);

    //@{
    ///\name IIdentifiable interface
    virtual const IdTag& getId() const override;
    virtual void setId(const IdTag&) override;
    //@}

    //@{
    ///\name IModificationAware interface
    virtual bool isDirty() const override;
    virtual void setDirty() override;
    virtual void setClean() override;
    //@}

private:
    HolyService(const HolyService&);
    HolyService& operator=(const HolyService&);

    QString m_intention;
	QDateTime m_startTime;
    QDateTime m_endTime;
    double m_receivedMoney;

    /// in 2011 a user wanted a marking that certain service could be
    /// moved to other date/time, that the client does not insist on fixed date
    TimeConstraint m_timeConstraint;

    IdTag m_clientId; ///< client for holy service
	IdTag m_churchId; ///< church where holy service is served
	IdTag m_priestId; ///< priest serving service

    Identifiable m_serviceId; ///< own service id
    BasicModificationAware m_modificationAware;
};
