#ifndef GOOGLECALENDARWIZARD_H
#define GOOGLECALENDARWIZARD_H

/*!
  \class GoogleCalendarWizard
  \brief User interface for synchronization of services with GoogleCalendar
  \author Jan 'Kovis' Struhar
*/

#include "HsTypedefs.h"

#include "localandremote.h"
#include "IdTag.h"

#include <QWizard>
#include <QDateTime>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QSet>
#include <QBuffer>
#include <QNetworkAccessManager>

#include <memory>

namespace Ui {
    class GoogleCalendarWizard;
}
class QNetworkReply;
class QNetworkRequest;
class QTcpServer;
class QTcpSocket;
class QJsonDocument;
class QJsonArray;
class Church;
class QUrlQuery;
class QTimer;
class QProgressBar;
class QTableView;
class LocalRemoteDiffModel;

class GoogleCalendarWizard : public QWizard
{
    Q_OBJECT
    typedef QWizard super;
public:
    explicit GoogleCalendarWizard(QWidget *parent = nullptr);
    ~GoogleCalendarWizard();

    void setYear(int year);

    QString accessToken() const;
    void setAccessToken(const QString &accessToken);

    QDateTime accessTokenValidTo() const;
    void setAccessTokenValidTo(const QDateTime &accessTokenValidTo);

    void setDefaultPriestId(const IdTag& defaultPriestId);

    virtual int nextId() const override;
    virtual bool validateCurrentPage() override;

signals:
    void networkActivity(bool isBusy);

protected:
    virtual void resizeEvent(QResizeEvent* e) override;

private slots:
    void handleRequestFinished(QNetworkReply* reply);
    void handleNewHttpConnection();
    void handleAuthenticationData();

    void handleCurrentPageChanged(int pageId);

    void onNetworkActivity(bool isBusy);

    void pc2webTransfer();
    void pc2webSendOneRequest();

    void web2pcTransfer();

    void on_startAutoSynchronization_clicked();

    void on_accessTokenResetBtn_clicked();
    void rebalanceTableColumns();
    static void rebalanceOneTable(QTableView& view);

private:
    Ui::GoogleCalendarWizard *m_ui = nullptr;
    QString m_accessToken;
    QDateTime m_accessTokenValidTo;
    QNetworkAccessManager* m_network = nullptr;
    QTcpServer* m_httpListener = nullptr;
    QTcpSocket* m_authConnection = nullptr;
    tHolyServiceVector m_localServices; ///< which services fall into date range here on local
    //@{
    ///\name Defaults for newly created services
    IdTag m_defaultPriestId;
    //@}

    enum AuthenticationPhase {
        INITIAL_STATE, AUTH_CODE_ACQUIRED, ACCESS_TOKEN_ACQUIRED, ERROR
    };
    AuthenticationPhase m_phase = INITIAL_STATE;

    enum PageId {
        INITIAL = 0, BROWSER, DIRECTION, PC2WEB, WEB2PC, RESOLVE_MODIFIED
    };

    void startAccessTokenAcquisition(const QString& code);
    void sendHttpResponse(const QString& msg);

    static bool extractGuid(const QString& str, QString& guid);
    static bool extractGuid(const QJsonObject& obj, const QString& key, QString& guid);
    static QNetworkRequest jsonRequest(const QString& bareUrl, const QUrlQuery& query);
    QUrlQuery accessTokenQuery() const;
    bool isAccessTokenValidNow() const;
    void setAccessTokenRelatedWidgets();

    void askForCalendarList();
    void askForEvents();
    void askForEvents(const QString& calendarId, const QString nextPageToken = QString());
    void createCalendarFor(const Church& aChurch);
    void processCalendarList(const QJsonArray& cals);
    void processCalendar(const QNetworkReply *reply, const QJsonObject& cal);
    void processEventList(const QNetworkReply *reply, const QJsonObject &eventsDocRoot);
    void processEvent(const QNetworkReply *reply, const QJsonObject& anEvent);

    QHash<QString, QString> m_churchToCalendarId;
    QSet<const QNetworkReply*> m_newCalendarRequests;
    QSet<const QNetworkReply*> m_eventListRequests;
    QSet<QString> m_eventRequests;

    QVector<RemoteEvent> m_remoteEvents;

    void addRemoteEvent(const QJsonObject& eventObj, const QString& calendarId);

    LocalRemoteDiffModel* m_pc2webModel = nullptr;
    void scanDifferences(QVector<LocalAndRemote>& localNew,
                         QVector<LocalAndRemote>& modified,
                         QVector<LocalAndRemote>& remoteNew,
                         QVector<LocalAndRemote>& remoteOrphaned) const;
    QString localToRemoteDiff(QVector<LocalAndRemote>& diffData) const;
    QString remoteToLocalDiff(QVector<LocalAndRemote>& diffData) const;
    QString autoSynchronizationDiff(QVector<LocalAndRemote>& diffData) const;

    void createRemoteEvent(const HolyService& hs);
    void updateRemoteEvent(const RemoteEvent& remote, const HolyService& hs, bool setServiceGuid = false);
    void removeRemoteEvent(const RemoteEvent& remote);

    /// buffer the requests as throttling on issuing too many commands at once
    struct OutgoingEventRequest {
        QNetworkRequest m_request;
        QString m_holyServiceGuid;
        QNetworkAccessManager::Operation m_operation = QNetworkAccessManager::PostOperation;
        bool m_removalRequest = false;
        QByteArray m_postedData;
        std::shared_ptr<QBuffer> m_postedDataBuffer;

        // be ready for repeated tries
        int m_attempts = 0;
    };
    QVector<OutgoingEventRequest> m_outEventRequests;
    QTimer* m_outgoingTimer = nullptr;

    static QString exportFormatted(const QDateTime& dt);
    static QDateTime parseDtInJsonSubKey(const QJsonValue& dtValue);
    static QDateTime parseDtString(const QString& dtIsoString);

    void repeatRequest(const QString& hsGuid);
    void removeOutgoingRequest(const QString& hsGuid);
    void removeRemoteEvent(const QString& hsGuid);
    void showDifferences();
    void advanceRemoteProgress();
    static void advanceProgressBar(QProgressBar& progress);

    void setDefaultChurch(const RemoteEvent &remote, HolyService& hs) const;
    void setDefaultPriest(HolyService& hs) const;

    void createNewLocalBasedOnRemote(const RemoteEvent& remote);
    static void setHolyServiceGuidToRemoteMessage(const HolyService &hs, QJsonObject &eventObj);

    enum UserInteraction {
        NO_USER_INTERVENTION, USER_CHANGED_LOCAL_SERVICE
    };
    UserInteraction transferDataFromRemote(const RemoteEvent& remote, HolyService& hs);
    UserInteraction tryToGuessClient(const RemoteEvent& remote, HolyService& hs) const;

    void initializeOutgoingTimer();
    void startSendingOutgoingMessages();

    /// update member variable m_localServices with services from current date range
    void updateLocalServices();

    void fillAutoSynchronizationPage();
};

#endif // GOOGLECALENDARWIZARD_H
