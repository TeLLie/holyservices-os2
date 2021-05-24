/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "GoogleCalendarWizard.h"
#include "ui_GoogleCalendarWizard.h"

#include "localremotediffmodel.h"
#include "UserSettings.h"
#include "HolyServiceIndexing.h"
#include "ChurchIndexing.h"
#include "ClientIndexing.h"
#include "ServiceDetailDlg.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTcpServer>
#include <QtDebug>
#include <QUrlQuery>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QTimer>
#include <QTimeZone>
#include <QDesktopServices>

const QString clientId("15619213899-6o4omt78q3cv2pb8c4p3er9893vfc6ge.apps.googleusercontent.com");
const QString clientSecret("OfFwARFEBJQNzNJ5WZDxsifd");

const QString kindKey("kind");
const QString accessTokenKey("access_token");

const QString idKey("id");
const QString itemsKey("items");
const QString descriptionKey("description");
const QString locationKey("location");
const QString hsGuidKey("HolyServicesGuid");
const QString dateTimeKey("dateTime");
const QString startKey("start");
const QString endKey("end");
const QString summaryKey("summary");

GoogleCalendarWizard::GoogleCalendarWizard(QWidget *parent) :
    QWizard(parent), m_ui(new Ui::GoogleCalendarWizard)
{
    m_ui->setupUi(this);

    connect(this, &GoogleCalendarWizard::currentIdChanged, this, &GoogleCalendarWizard::handleCurrentPageChanged);
    connect(this, &GoogleCalendarWizard::networkActivity, this, &GoogleCalendarWizard::onNetworkActivity);
    connect(m_ui->pc2webTransferBtn, &QAbstractButton::clicked, this, &GoogleCalendarWizard::pc2webTransfer);
    connect(m_ui->web2pcTransferBtn, &QAbstractButton::clicked, this, &GoogleCalendarWizard::web2pcTransfer);

    m_ui->googleMailEdit->setText(UserSettings::get().googleAccount());
}

GoogleCalendarWizard::~GoogleCalendarWizard()
{
    delete m_ui;
}

void GoogleCalendarWizard::setYear(int year)
{
    m_ui->fromDt->setDate(QDate(year, 1, 1));
    m_ui->toDt->setDate(QDate(year, 12, 31));
}

QString GoogleCalendarWizard::accessToken() const
{
    return m_accessToken;
}

void GoogleCalendarWizard::setAccessToken(const QString &accessToken)
{
    m_accessToken = accessToken;
}

QDateTime GoogleCalendarWizard::accessTokenValidTo() const
{
    return m_accessTokenValidTo;
}

void GoogleCalendarWizard::setAccessTokenValidTo(const QDateTime &accessTokenValidTo)
{
    m_accessTokenValidTo = accessTokenValidTo;
}

int GoogleCalendarWizard::nextId() const
{
    switch (currentId())
    {
    case INITIAL:
        if (! isAccessTokenValidNow())
        {
            return BROWSER;
        }
    [[clang::fallthrough]];
    case BROWSER:
        return DIRECTION;
    case DIRECTION:
        if (m_ui->pc2web->isChecked())
        {
            return PC2WEB;
        }
        else if (m_ui->web2pc->isChecked())
        {
            return WEB2PC;
        }
        return RESOLVE_MODIFIED;
    case WEB2PC:
    case PC2WEB:
        return -1;
    }
    return -1;
}

bool GoogleCalendarWizard::validateCurrentPage()
{
    switch (currentId())
    {
    case BROWSER:
        return isAccessTokenValidNow();
    default:
        break;
    }
    return true;
}

void GoogleCalendarWizard::handleNewHttpConnection()
{
    if (! m_httpListener->hasPendingConnections()) return;

    m_authConnection = m_httpListener->nextPendingConnection();
    connect(m_authConnection, &QIODevice::readyRead, this, &GoogleCalendarWizard::handleAuthenticationData);
}

void GoogleCalendarWizard::handleAuthenticationData()
{
    const auto authReply = m_authConnection->readAll();
    qDebug() << "AUTH RESPONSE2: " << authReply;

    const QRegularExpression urlRe("GET\\s+(?<url>\\S+)\\s+HTTP");
    const auto authReplyText = QString::fromUtf8(authReply);
    const QRegularExpressionMatch match = urlRe.match(authReplyText);
    if (! match.hasMatch())
    {
        qDebug() << "Auth reply does not contain URL behind GET";
        return;
    }
    const auto getPayload = match.captured("url");
    qDebug() << "GET payload: " << getPayload;
    const QUrlQuery urlQuery((QUrl(getPayload)));
    QString interestingKey;

    interestingKey = "error";
    if (urlQuery.hasQueryItem(interestingKey))
    {
        sendHttpResponse(tr("Error acquiring authentication: %1").
                         arg(urlQuery.queryItemValue(interestingKey)));
        m_phase = ERROR;
        return;
    }

    interestingKey = "code";
    if (urlQuery.hasQueryItem(interestingKey))
    {
        const auto authCode = urlQuery.queryItemValue(interestingKey);
        m_phase = AUTH_CODE_ACQUIRED;
        startAccessTokenAcquisition(authCode);
        sendHttpResponse(tr("Authentication was successful, proceed with GoogleCalendar synchronization in the application."));
    }
    else
    {
        sendHttpResponse(tr("Strange unexpected authentication behavior."));
    }
}

void GoogleCalendarWizard::sendHttpResponse(const QString &msg)
{
    const auto msgUtf8 = msg.toUtf8();
    m_authConnection->write(QString(
        "HTTP/1.1 200 OK\n"
        "Content-Type: text/plain; charset=utf-8\n"
        "Content-Length: %1\n\n"
        "%2").arg(msgUtf8.size()).arg(msg).toUtf8());
}

void GoogleCalendarWizard::fillAutoSynchronizationPage()
{
    QVector<LocalAndRemote> diffData;
    const auto summary = autoSynchronizationDiff(diffData);
    m_ui->autoSynchroEdit->setText(summary);
    m_ui->startAutoSynchronization->setEnabled(diffData.size());
    if (diffData.size()) m_ui->autoSynchroProgress->setRange(0, diffData.size());
    m_ui->autoSynchroProgress->setValue(0);
}

void GoogleCalendarWizard::handleCurrentPageChanged(int pageId)
{
    qDebug() << "Wizard page " << currentId();
    switch (pageId)
    {
    case INITIAL:
        m_ui->tokenValidityEdit->setTime(m_accessTokenValidTo.time());
        setAccessTokenRelatedWidgets();

        // after wizard is closed and reopened again, mov the state directly to access token OK
        if (isAccessTokenValidNow() && (m_phase == INITIAL_STATE))
        {
            m_phase = ACCESS_TOKEN_ACQUIRED;
        }
        if (m_network == nullptr)
        {
            m_network = new QNetworkAccessManager(this);
            connect(m_network, &QNetworkAccessManager::finished,
                    this, &GoogleCalendarWizard::handleRequestFinished);
        }
        break;
    case BROWSER:
        if (m_httpListener == nullptr)
        {
            m_httpListener = new QTcpServer(this);
            connect(m_httpListener, &QTcpServer::newConnection,
                    this, &GoogleCalendarWizard::handleNewHttpConnection);
        }

        {
            bool listenOk = m_httpListener->isListening();
            if (m_ui->authenticationPort->value() != m_httpListener->serverPort())
            {
                listenOk = false; // change listen port
            }
            if (! listenOk)
            {
                listenOk = m_httpListener->listen(
                    QHostAddress::Any,
                    static_cast<quint16>(m_ui->authenticationPort->value()));
            }
            if (! listenOk)
            {
                QMessageBox::warning(this, tr("Occupied port"),
                    tr("Listening on port %1 failed.\n"
                       "Please choose different port or allow port in firewall.").
                                     arg(m_ui->authenticationPort->value()));
                return;
            }
        }

        UserSettings::get().setGoogleAccount(m_ui->googleMailEdit->text());

        {
            QUrlQuery query;
            query.addQueryItem("client_id", clientId);
            query.addQueryItem("redirect_uri", QString("http://localhost:%1").arg(m_ui->authenticationPort->value()));
            query.addQueryItem("response_type", "code");
            query.addQueryItem("login_hint", m_ui->googleMailEdit->text());
            query.addQueryItem("scope", "https://www.googleapis.com/auth/calendar");
            QUrl url("https://accounts.google.com/o/oauth2/v2/auth");
            url.setQuery(query);

            qDebug() << "Browser load " << url;

            //m_ui->browser->load(url);
            QDesktopServices::openUrl(url);
        }
        break;
    case DIRECTION:
        updateLocalServices();
        askForCalendarList();
        break;
    case WEB2PC:
    case PC2WEB:
        rebalanceTableColumns();
        break;
    case RESOLVE_MODIFIED:
        fillAutoSynchronizationPage();
        break;
    }
}

void GoogleCalendarWizard::startAccessTokenAcquisition(const QString &code)
{
    QNetworkRequest postRequest(QUrl("https://accounts.google.com/o/oauth2/token"));
    postRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QUrlQuery params;
    params.addQueryItem("code", QUrl::toPercentEncoding(code));
    params.addQueryItem("client_id", QUrl::toPercentEncoding(clientId));
    params.addQueryItem("client_secret", QUrl::toPercentEncoding(clientSecret));
    params.addQueryItem("redirect_uri", QUrl::toPercentEncoding("http://localhost:1500"));
    params.addQueryItem("grant_type", QUrl::toPercentEncoding("authorization_code"));
    m_network->post(postRequest, params.query().toUtf8());
}

bool GoogleCalendarWizard::extractGuid(const QString &str, QString &guid)
{
    static const QRegularExpression re("ID:\\s*(?<guid>{\\S+})");
    auto match = re.match(str);
    if (match.hasMatch())
    {
        guid = match.captured("guid");
        return true;
    }
    return false;
}

bool GoogleCalendarWizard::extractGuid(const QJsonObject &obj, const QString &key, QString &guid)
{
    if (! obj.contains(key)) return false;
    return extractGuid(obj.value(key).toString(), guid);
}

void GoogleCalendarWizard::handleRequestFinished(QNetworkReply *reply)
{
    qDebug() << "reply finished";
    const QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> replyDestroyer(reply);
    const auto replyContents = reply->readAll();
    const auto doc = QJsonDocument::fromJson(replyContents);
    const auto replyGuid = reply->objectName();

    qDebug() << "reply contents: " << replyContents;

    switch (m_phase)
    {
    case AUTH_CODE_ACQUIRED:
        if (doc.object().contains(accessTokenKey))
        {
            m_accessToken = doc.object().value(accessTokenKey).toString();
            if (m_accessToken.length())
            {
                m_phase = ACCESS_TOKEN_ACQUIRED;
                qDebug() << "ACCESS_TOKEN: " << m_accessToken;

                const std::chrono::seconds tokenDuration(doc.object().value("expires_in").toInt());
                m_accessTokenValidTo = QDateTime::currentDateTime().addSecs(tokenDuration.count());

                next(); // advance to next page
            }
        }
        break;
    case ACCESS_TOKEN_ACQUIRED:
        {
            const QString kind = doc.object().value(kindKey).toString();

            if (kind == "calendar#calendarList")
            {
                emit networkActivity(false);
                processCalendarList(doc.object().value(itemsKey).toArray());
                askForEvents();
            }
            else if (kind == "calendar#calendar")
            {
                processCalendar(reply, doc.object());
                askForEvents();
            }
            else if (kind == "calendar#events")
            {
                processEventList(reply, doc.object());
            }
            else if (kind == "calendar#event")
            {
                processEvent(reply, doc.object());
            }
            else
            {
                // delete requests on remote events
                if (reply->operation() == QNetworkAccessManager::DeleteOperation)
                {
                    if (replyContents.trimmed().isEmpty())
                    {
                        removeRemoteEvent(replyGuid);
                        removeOutgoingRequest(replyGuid);
                        advanceRemoteProgress();
                    }
                    else
                    {
                        repeatRequest(replyGuid);
                    }
                }
                else
                {
                    // error message handling
                    // if the request was of holy service, send the request again
                    repeatRequest(replyGuid);
                }
            }
        }
        break;
    default: break;
    }
    m_eventRequests.remove(replyGuid);

    {
        const bool calendarRequestsRunning = (! m_newCalendarRequests.isEmpty());
        m_newCalendarRequests.remove(reply);
        if (m_newCalendarRequests.isEmpty() && calendarRequestsRunning)
        {
            emit networkActivity(false);
        }
    }
}

void GoogleCalendarWizard::repeatRequest(const QString& hsGuid)
{
    auto hsFinder = [hsGuid](const OutgoingEventRequest& req) {
        return (req.m_holyServiceGuid == hsGuid); };
    auto it = std::find_if(m_outEventRequests.begin(), m_outEventRequests.end(), hsFinder);
    if (it != m_outEventRequests.end())
    {
        OutgoingEventRequest& req = *it;
        ++req.m_attempts;
    }
    else
    {
        qDebug() << "STRANGE " << hsGuid;
    }
}

void GoogleCalendarWizard::removeRemoteEvent(const QString& hsGuid)
{
    QMutableVectorIterator<RemoteEvent> it(m_remoteEvents);
    while (it.hasNext())
    {
        const auto& remote = it.next();
        if (remote.m_holyServiceGuid == hsGuid)
        {
            it.remove();
        }
    }
}

void GoogleCalendarWizard::askForCalendarList()
{
    const auto urlText = QString("https://www.googleapis.com/calendar/v3/users/me/calendarList?access_token=%1").
            arg(m_accessToken);
    QNetworkRequest req((QUrl(urlText)));
    m_network->get(req);
    emit networkActivity(true);
}

void GoogleCalendarWizard::askForEvents()
{
    if (! m_newCalendarRequests.isEmpty()) return;

    m_remoteEvents.clear();
    const auto calendarIds = m_churchToCalendarId.values();
    for (const auto& oneCalendarId : calendarIds)
    {
        askForEvents(oneCalendarId, QString());
    }
    if (! m_eventListRequests.isEmpty())
    {
        emit networkActivity(true);
    }
}

void GoogleCalendarWizard::askForEvents(const QString& calendarId, const QString nextPageToken)
{
    QUrl url(QString("https://www.googleapis.com/calendar/v3/calendars/%1/events").arg(calendarId));
    auto urlQuery = accessTokenQuery();
    if (! nextPageToken.isEmpty())
    {
        urlQuery.addQueryItem("pageToken", nextPageToken);
    }
    url.setQuery(urlQuery);

    QNetworkRequest req(url);
    auto reply = m_network->get(req);
    reply->setObjectName(calendarId);
    m_eventListRequests.insert(reply);
}

void GoogleCalendarWizard::processCalendarList(const QJsonArray &cals)
{
    QSet<QString> localChurchesGuids;
    auto allChurches = ChurchIndexing::allSet().data();
    for (const auto& church : allChurches)
    {
        localChurchesGuids.insert(church->getId().toString());
    }

    m_churchToCalendarId.clear();

    QSet<QString> remoteCalendarGuids;

    // create calendars that do not exist yet
    for (const auto& calValue : cals)
    {
        const auto calObject = calValue.toObject();
        const auto calId = calObject.value(idKey).toString();
        QString churchGuid;
        if (extractGuid(calObject, descriptionKey, churchGuid))
        {
            m_churchToCalendarId[churchGuid] = calId;
            remoteCalendarGuids.insert(churchGuid);
        }
    }
    const QSet<QString> notExisting = localChurchesGuids - remoteCalendarGuids;
    for (const auto& church : allChurches)
    {
        if (notExisting.contains(church->getId().toString()))
        {
            createCalendarFor(*church);
        }
    }
}

void GoogleCalendarWizard::createCalendarFor(const Church &aChurch)
{
    const QNetworkRequest req = jsonRequest("https://www.googleapis.com/calendar/v3/calendars",
                                      accessTokenQuery());
    QJsonObject calendar;
    auto calendarName = aChurch.name().trimmed();

    // calendar HAS to have a name
    if (calendarName.isEmpty()) calendarName = tr("unnamed church");

    calendar[summaryKey] = calendarName;

    qDebug() << "createCalendarFor: " << aChurch.name();

    auto reply = m_network->post(req, QJsonDocument(calendar).toJson());
    reply->setObjectName(aChurch.getId().toString());
    m_newCalendarRequests.insert(reply);
    emit networkActivity(true);
}

void GoogleCalendarWizard::processCalendar(const QNetworkReply *reply, const QJsonObject &cal)
{
    if (! m_newCalendarRequests.contains(reply)) return;

    auto churchGuid = reply->objectName();
    m_newCalendarRequests.remove(reply);
    if (churchGuid.isEmpty())
    {
        extractGuid(cal, descriptionKey, churchGuid);
        m_churchToCalendarId[churchGuid] = cal.value(idKey).toString();

        if (m_newCalendarRequests.isEmpty())
        {
            emit networkActivity(false);
        }
        return; // two step calendar creation finished, no further update needed
    }

    // fill data into newly created calendar record
    const auto churchTag = IdTag::createFromString(churchGuid);
    const auto aChurch = ChurchIndexing::allSet().find(churchTag);
    if (aChurch == nullptr) return;

    qDebug() << "update calendar for: " << aChurch->name();

    auto updatedCalendar = cal;
    updatedCalendar[locationKey] = aChurch->place();
    updatedCalendar[descriptionKey] = QString("ID: %1").arg(churchGuid);
    updatedCalendar["timeZone"] = QString::fromUtf8(QTimeZone::systemTimeZoneId());

    const auto bareUrl = QString("https://www.googleapis.com/calendar/v3/calendars/%1").
            arg(cal.value(idKey).toString());
    auto req = jsonRequest(bareUrl, accessTokenQuery());
    const auto updatingReply = m_network->put(req, QJsonDocument(updatedCalendar).toJson());
    m_newCalendarRequests.insert(updatingReply);
}

void GoogleCalendarWizard::addRemoteEvent(const QJsonObject& eventObj, const QString& calendarId)
{
    RemoteEvent newEvent;

    newEvent.m_eventId = eventObj.value(idKey).toString();
    newEvent.m_holyServiceGuid = eventObj.value("extendedProperties").
            toObject().value("private").toObject().value(hsGuidKey).toString();

    newEvent.m_calendarId = calendarId;
    newEvent.m_summary = eventObj.value(summaryKey).toString();
    newEvent.m_description = eventObj.value(descriptionKey).toString();
    newEvent.m_start = parseDtInJsonSubKey(eventObj.value(startKey));
    newEvent.m_end = parseDtInJsonSubKey(eventObj.value(endKey));
    newEvent.m_recentlyModified = parseDtString(eventObj.value("updated").toString());

    if (newEvent.m_holyServiceGuid.isEmpty())
    {
        qDebug() << newEvent.m_summary;
    }
    if (newEvent.isValid())
    {
        const auto oldVersionOfEventIt =
            std::find_if(std::begin(m_remoteEvents), std::end(m_remoteEvents),
                [&newEvent](const RemoteEvent& e) { return newEvent.m_eventId == e.m_eventId;});
        if (oldVersionOfEventIt == std::end(m_remoteEvents))
        {
            m_remoteEvents.append(newEvent);
        }
        else
        {
            *oldVersionOfEventIt = newEvent;
        }
    }
}

void GoogleCalendarWizard::processEventList(const QNetworkReply* reply, const QJsonObject &eventsDocRoot)
{
    m_eventListRequests.remove(reply);

    const auto calendarId = reply->objectName();

    {
        const auto eventArray = eventsDocRoot.value(itemsKey).toArray();
        for (const auto& eventVal : eventArray)
        {
            const auto eventObj = eventVal.toObject();

            addRemoteEvent(eventObj, calendarId);
        }
    }

    // are there next results for this calendar to be asked?
    const auto nextPageToken = eventsDocRoot.value(QStringLiteral("nextPageToken")).toString();
    if (! nextPageToken.isEmpty())
    {
        askForEvents(calendarId, nextPageToken);
    }

    if (m_eventListRequests.isEmpty())
    {
        emit networkActivity(false);
    }
}

void GoogleCalendarWizard::processEvent(const QNetworkReply* reply, const QJsonObject &anEvent)
{
    const auto replyGuid = reply->objectName();
    removeOutgoingRequest(replyGuid);
    addRemoteEvent(anEvent, anEvent.value("organizer").toObject().value("email").toString());
    advanceRemoteProgress();
}

void GoogleCalendarWizard::advanceRemoteProgress()
{
    switch (currentId())
    {
    case PC2WEB:
        advanceProgressBar(*m_ui->pc2webProgress);
        break;
    case WEB2PC:
        advanceProgressBar(*m_ui->web2pcProgress);
        break;
    case RESOLVE_MODIFIED:
        advanceProgressBar(*m_ui->autoSynchroProgress);
        break;
    }
}

void GoogleCalendarWizard::advanceProgressBar(QProgressBar &progress)
{
    progress.setValue(progress.value() + 1);
}

void GoogleCalendarWizard::removeOutgoingRequest(const QString& hsGuid)
{
    QMutableVectorIterator<OutgoingEventRequest> it(m_outEventRequests);
    while (it.hasNext())
    {
        const auto& req = it.next();
        if (req.m_holyServiceGuid == hsGuid)
        {
            qDebug() << "Removing: " << hsGuid;
            it.remove();
        }
    }
}

void GoogleCalendarWizard::onNetworkActivity(bool isBusy)
{
    for (auto btn : {m_ui->web2pcTransferBtn, m_ui->pc2webTransferBtn, m_ui->startAutoSynchronization})
    {
        btn->setEnabled(! isBusy);
    }

    if (isBusy)
    {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    }
    else
    {
        updateLocalServices();
        showDifferences();

        // reset the overriden waiting cursor
        while (QApplication::overrideCursor()) QApplication::restoreOverrideCursor();
    }
}

void GoogleCalendarWizard::showDifferences()
{
    {
        QVector<LocalAndRemote> diffData;

        const auto diffText = localToRemoteDiff(diffData);
        m_ui->pc2webEdit->setText(diffText);

        if (m_pc2webModel == nullptr)
        {
            m_pc2webModel = new LocalRemoteDiffModel(this);
            m_ui->pc2webTable->setModel(m_pc2webModel);
            m_ui->web2pcTable->setModel(m_pc2webModel); // the same stuff shown
            m_ui->autoSynchroTable->setModel(m_pc2webModel);
        }
        m_pc2webModel->setDiffData(diffData);
    }

    {
        QVector<LocalAndRemote> diffData;
        const auto diffText = remoteToLocalDiff(diffData);
        m_ui->web2pcEdit->setText(diffText);
    }

    {
        QVector<LocalAndRemote> diffData;
        const auto diffText = autoSynchronizationDiff(diffData);
        m_ui->autoSynchroEdit->setText(diffText);
    }

    rebalanceTableColumns();
}

QString GoogleCalendarWizard::localToRemoteDiff(QVector<LocalAndRemote>& diffData) const
{
    QVector<LocalAndRemote> localNew, modified, remoteNew, remoteOrphaned;
    scanDifferences(localNew, modified, remoteNew, remoteOrphaned);

    diffData = localNew + modified + remoteNew;

    QString retVal;
    if (! diffData.isEmpty())
    {
        if (localNew.size())
        {
            retVal.append(tr("On local side are %1 new masses that will be transferred to the internet.").arg(localNew.size()));
        }
        if (modified.size())
        {
            if (! retVal.isEmpty()) retVal += "\n";
            retVal += tr("%1 different masses.").arg(modified.size());
        }
        if (remoteNew.size() + remoteOrphaned.size())
        {
            if (! retVal.isEmpty()) retVal += "\n";
            retVal += tr("On remote side are %1 orphaned entries that will be deleted.").
                    arg(remoteNew.size() + remoteOrphaned.size());
        }
    }
    else
    {
        retVal = tr("It seems that there is no difference between local and remote side.");
    }
    return retVal;
}

QString GoogleCalendarWizard::remoteToLocalDiff(QVector<LocalAndRemote> &diffData) const
{
    QVector<LocalAndRemote> localNew, modified, remoteNew, remoteOrphaned;

    scanDifferences(localNew, modified, remoteNew, remoteOrphaned);

    diffData = localNew + modified + remoteNew + remoteOrphaned;

    QString retVal;
    if (localNew.size() || remoteNew.size() || remoteOrphaned.size() || modified.size())
    {
        if (localNew.size())
        {
            retVal.append(tr("On local side are %1 new masses that do not exist in web calendars."
                             " Local masses will be left <b>untouched</b>.").arg(localNew.size()));
        }
        if (modified.size())
        {
            if (! retVal.isEmpty()) retVal += "<br>";
            retVal += tr("%1 different masses, changes will be synchronized.").arg(modified.size());
        }
        if (remoteNew.size())
        {
            if (! retVal.isEmpty()) retVal += "<br>";
            retVal += tr("On remote side are %1 masses that do not exist on local computer."
                         " They will be transferred to local storage.").arg(remoteNew.size());
        }
        if (remoteOrphaned.size())
        {
            if (! retVal.isEmpty()) retVal += "<br>";
            retVal += tr("On remote side are %1 masses that once used to exist on local computer but were deleted on local side."
                         " No action will be taken.").arg(remoteNew.size());
        }
    }
    else
    {
        retVal = tr("It seems that there is no difference between local and remote side.");
    }
    return retVal;
}

QString GoogleCalendarWizard::autoSynchronizationDiff(QVector<LocalAndRemote>& diffData) const
{
    QVector<LocalAndRemote> localNew, modified, remoteNew, remoteOrphaned;

    scanDifferences(localNew, modified, remoteNew, remoteOrphaned);

    // auto-synchronization shall send local new to web,
    // modified items according to date of recent update
    // remote new are transferred into local storage
    // and removes remote orphaned
    diffData = localNew + modified + remoteNew + remoteOrphaned;

    QString retVal;
    if (diffData.size())
    {
        if (localNew.size())
        {
            retVal += tr("%1 local services will be transferred to GoogleCalendar.").arg(localNew.size());
        }
        if (modified.size())
        {
            if (! retVal.isEmpty()) retVal.append("<br>");
            retVal += tr("There are %1 different records.").arg(modified.size());

            int webToPc = 0, pcToWeb = 0;
            for (const auto& events : modified)
            {
                const auto localModifiedUtc = events.m_local->lastModified().toUTC();
                if (localModifiedUtc < events.m_remote.m_recentlyModified)
                {
                    ++webToPc;
                }
                else if (localModifiedUtc > events.m_remote.m_recentlyModified)
                {
                    ++pcToWeb;
                }
            }
            if (webToPc)
            {
                retVal.append("<br>");
                retVal += tr("* from GoogleCalendar to this PC will be updated %1 records").arg(webToPc);
            }
            if (pcToWeb)
            {
                retVal.append("<br>");
                retVal += tr("* from this PC to GoogleCalendar will be updated %1 records").arg(pcToWeb);
            }
        }
        if (remoteNew.size())
        {
            if (! retVal.isEmpty()) retVal.append("<br>");
            retVal += tr("%1 GoogleCalendar entries will be tranferred to this PC.").arg(remoteNew.size());
        }
        if (remoteOrphaned.size())
        {
            if (! retVal.isEmpty()) retVal.append("<br>");
            retVal += tr("%1 orphaned GoogleCalendar entries will be <font color=\"red\">deleted</font>.").arg(remoteOrphaned.size());
        }
        return retVal;
    }
    return tr("Nothing can be auto-synchronized.");
}

void GoogleCalendarWizard::scanDifferences(
        QVector<LocalAndRemote>& localNew,
        QVector<LocalAndRemote>& modified,
        QVector<LocalAndRemote>& remoteNew,
        QVector<LocalAndRemote>& remoteOrphaned) const
{
    localNew.clear(); modified.clear(); remoteNew.clear(); remoteOrphaned.clear();

    QHash<QString, LocalAndRemote> guidToEvents;
    for (const auto& event : m_remoteEvents)
    {
        if ((event.m_start.date() < m_ui->fromDt->date()) || (event.m_end.date() > m_ui->toDt->date()))
        {
            continue; // skip those not from date range
        }

        if (event.m_holyServiceGuid.isEmpty())
        {
            LocalAndRemote newEntry;
            newEntry.m_remote = event;
            remoteNew.append(newEntry);
        }
        else
        {
            guidToEvents[event.m_holyServiceGuid].m_remote = event;
        }
    }
    for (const auto& localHs : m_localServices)
    {
        guidToEvents[localHs->getId().toString()].m_local = localHs;
    }

    QHashIterator<QString, LocalAndRemote> it(guidToEvents);
    while (it.hasNext())
    {
        it.next();
        const auto& events = it.value();
        if (events.m_local && (! events.m_remote.isValid()))
        {
            localNew.append(events);
        }
        else if ((events.m_local == nullptr) && (events.m_remote.isValid()))
        {
            if (! events.m_remote.m_holyServiceGuid.isEmpty())
            {
                remoteOrphaned.append(events);
            }
        }
        else if (events.m_local)
        {
            if (! (events.m_remote == *events.m_local))
            {
                modified.append(events);
            }
        }
    }

    // sort by local start date if local is available, by remote start date otherwise
    auto sortByStart = [](const LocalAndRemote& r1, const LocalAndRemote& r2) {
        if (r1.m_local && r2.m_local) return (r1.m_local->startTime() < r2.m_local->startTime());
        return (r1.m_remote.m_start < r2.m_remote.m_start);
    };
    std::sort(localNew.begin(), localNew.end(), sortByStart);
    std::sort(remoteNew.begin(), remoteNew.end(), sortByStart);
    std::sort(remoteOrphaned.begin(), remoteOrphaned.end(), sortByStart);
    std::sort(modified.begin(), modified.end(), sortByStart);
}

void GoogleCalendarWizard::initializeOutgoingTimer()
{
    if (m_outgoingTimer) return;

    m_outgoingTimer = new QTimer(this);
    m_outgoingTimer->setInterval(100);
    m_outgoingTimer->setSingleShot(false);
    connect(m_outgoingTimer, &QTimer::timeout, this, &GoogleCalendarWizard::pc2webSendOneRequest);
}

void GoogleCalendarWizard::startSendingOutgoingMessages()
{
    initializeOutgoingTimer();
    m_outgoingTimer->start();
    qDebug() << "Starting sending outgoing requests";
    emit networkActivity(true);
}

void GoogleCalendarWizard::pc2webTransfer()
{
    qDebug() << "pc2webTransfer";

    QVector<LocalAndRemote> localNew, modified, remoteNew, remoteOrphaned;
    scanDifferences(localNew, modified, remoteNew, remoteOrphaned);

    for (const auto& diff : localNew)
    {
        createRemoteEvent(*diff.m_local);
    }
    for (const auto& diff : modified)
    { // remote events that have local guids refresh to correspond with local state
        updateRemoteEvent(diff.m_remote, *diff.m_local);
    }

    // remove remote events that do not have local counterpart
    const auto allRemote = remoteNew + remoteOrphaned;
    for (const auto& diff : allRemote)
    {
        removeRemoteEvent(diff.m_remote);
    }

    if (m_outEventRequests.isEmpty())
    {
        QMessageBox::information(this, tr("Information"), tr("Sorry there is no difference to be transferred."));
        return;
    }
    m_ui->pc2webProgress->setMaximum(m_outEventRequests.size());
    m_ui->pc2webProgress->setValue(0);

    startSendingOutgoingMessages();
}

void GoogleCalendarWizard::pc2webSendOneRequest()
{
    if (m_outEventRequests.isEmpty() && m_eventRequests.isEmpty())
    {
        m_outgoingTimer->stop();
        emit networkActivity(false);
        qDebug() << "Stopping sending outgoing requests";
        return;
    }

    // get in the front the least (or not yet) sent requests
    auto attemptSorter = [](const OutgoingEventRequest& r1, const OutgoingEventRequest& r2) {
        return (r1.m_attempts < r2.m_attempts); };
    std::sort(m_outEventRequests.begin(), m_outEventRequests.end(), attemptSorter);

    for (OutgoingEventRequest& toBeSent : m_outEventRequests)
    {
        if (m_eventRequests.contains(toBeSent.m_holyServiceGuid)) continue;

        ++toBeSent.m_attempts;
        switch (toBeSent.m_operation)
        {
        case QNetworkAccessManager::PostOperation:
            {
                qDebug() << "POST " << toBeSent.m_postedData;
                auto reply = m_network->post(toBeSent.m_request, toBeSent.m_postedData);
                reply->setObjectName(toBeSent.m_holyServiceGuid);
                m_eventRequests.insert(reply->objectName());
            }
            break;
        case QNetworkAccessManager::CustomOperation: // PATCH
            {
                qDebug() << "PATCH " << toBeSent.m_postedData;
                auto reply = m_network->sendCustomRequest(
                            toBeSent.m_request, QByteArray("PATCH"), toBeSent.m_postedDataBuffer.get());
                reply->setObjectName(toBeSent.m_holyServiceGuid);
                m_eventRequests.insert(reply->objectName());
            }
            break;
        case QNetworkAccessManager::DeleteOperation:
            {
                qDebug() << "DELETE " << toBeSent.m_request.url();
                auto reply = m_network->deleteResource(toBeSent.m_request);
                reply->setObjectName(toBeSent.m_holyServiceGuid);
                m_eventRequests.insert(reply->objectName());
            }
            break;
        default: break;
        }
    }
}

void GoogleCalendarWizard::web2pcTransfer()
{
    qDebug() << "web2pcTransfer localServices: " << m_localServices.size() << " remote: " << m_remoteEvents.size();

    QVector<LocalAndRemote> localNew, modified, remoteNew, remoteOrphaned;
    scanDifferences(localNew, modified, remoteNew, remoteOrphaned);

    const auto differing = modified + remoteNew;


    if (differing.isEmpty())
    {
        QMessageBox::information(this, tr("Information"),
            tr("Sorry there is no known difference between remote and local data.\n\n"
               "Start wizard again if you want to have most fresh information."));
        return;
    }

    m_ui->web2pcProgress->setMaximum(differing.size());
    m_ui->web2pcProgress->setValue(0);

    for (auto& diff : modified)
    {
        transferDataFromRemote(diff.m_remote, *diff.m_local);
        advanceProgressBar(*m_ui->web2pcProgress);
    }
    for (auto& diff : remoteNew)
    { // create new mass record - for current church, priest
        createNewLocalBasedOnRemote(diff.m_remote);
        // progress advanced as response about successfull GUId setting on remote record comes
    }

    updateLocalServices();
    showDifferences();

    startSendingOutgoingMessages();
}

void GoogleCalendarWizard::createNewLocalBasedOnRemote(const RemoteEvent &remote)
{
    std::unique_ptr<HolyService> newService(new HolyService);
    newService->setId(IdTag::createInitialized());

    transferDataFromRemote(remote, *newService);

    HolyService* dummy = nullptr;
    if (HolyServiceIndexing::allSet().addElement(newService.get(), dummy))
    { // update the remote event that it holds newly created service guid
        updateRemoteEvent(remote, *newService, true);
        newService.release();
    }
}

GoogleCalendarWizard::UserInteraction GoogleCalendarWizard::transferDataFromRemote(
        const RemoteEvent &remote, HolyService &hs)
{
    hs.setStartTime(remote.m_start);
    hs.setEndTime(remote.m_end);

    setDefaultPriest(hs);
    setDefaultChurch(remote, hs);

    if (UserSettings::get().isClientImportant())
    {
        hs.setIntention(remote.m_description);

        if (remote.m_summary.trimmed().isEmpty())
        {
            hs.setClient(nullptr);
        }
        else if (remote.m_summary != LocalAndRemote::summaryLine(hs))
        {
            return tryToGuessClient(remote, hs);
        }
    }
    else
    {
        hs.setIntention(remote.m_summary);

        if (remote.m_description.trimmed().isEmpty())
        {
            hs.setClient(nullptr);
        }
        else if (remote.m_description != LocalAndRemote::descriptionLine(hs))
        {
            return tryToGuessClient(remote, hs);
        }
    }
    return NO_USER_INTERVENTION;
}

GoogleCalendarWizard::UserInteraction GoogleCalendarWizard::tryToGuessClient(
        const RemoteEvent& remote, HolyService& hs) const
{
    auto allClients = ClientIndexing::allSet().data();

    if (UserSettings::get().isClientImportant())
    {
        for (const Client* aClient : allClients)
        {
            if (aClient->toString(Client::SurnameName) == remote.m_summary)
            {
                hs.setClient(aClient);
                return NO_USER_INTERVENTION;
            }
        }

        // ask GUI to decide
        ServiceDetailDlg serviceDlg(const_cast<GoogleCalendarWizard*>(this));

        serviceDlg.setWindowTitle(tr("Please assign the client"));
        serviceDlg.setNewClientNameHint(remote.m_summary);
        serviceDlg.setService(&hs);

        return (serviceDlg.exec() == QDialog::Accepted) ? USER_CHANGED_LOCAL_SERVICE : NO_USER_INTERVENTION;
    }
    else
    {
        const auto clientHint = remote.m_description.trimmed();
        for (const Client* aClient : allClients)
        {
            if (aClient->toString(Client::SurnameName).trimmed() == clientHint)
            {
                hs.setClient(aClient);
                return NO_USER_INTERVENTION;
            }
        }

        // ask GUI to decide
        ServiceDetailDlg serviceDlg(const_cast<GoogleCalendarWizard*>(this));

        serviceDlg.setWindowTitle(tr("Please assign the client"));
        serviceDlg.setNewClientNameHint(remote.m_description);
        serviceDlg.setService(&hs);

        return (serviceDlg.exec() == QDialog::Accepted) ? USER_CHANGED_LOCAL_SERVICE : NO_USER_INTERVENTION;
    }
}

void GoogleCalendarWizard::setHolyServiceGuidToRemoteMessage(const HolyService &hs, QJsonObject& eventObj)
{
    QJsonObject extPropObj, privateObj;
    privateObj[hsGuidKey] = hs.getId().toString();
    extPropObj["private"] = privateObj;
    eventObj["extendedProperties"] = extPropObj;
}

void GoogleCalendarWizard::createRemoteEvent(const HolyService &hs)
{
    const auto church = hs.getChurch();
    if (! church) return;

    const auto churchGuid = church->getId().toString();
    const auto calendarId = m_churchToCalendarId.value(churchGuid);

    if (calendarId.isEmpty()) return;

    auto req = jsonRequest(QString("https://www.googleapis.com/calendar/v3/calendars/%1/events").
                                      arg(calendarId), accessTokenQuery());
    QJsonObject eventObj;
    eventObj[locationKey] = church->place();

    eventObj[summaryKey] = LocalAndRemote::summaryLine(hs);
    eventObj[descriptionKey] = LocalAndRemote::descriptionLine(hs);

    {
        QJsonObject startObj;
        startObj[dateTimeKey] = exportFormatted(hs.startTime());
        eventObj[startKey] = startObj;
    }
    {
        QJsonObject endObj;
        endObj[dateTimeKey] = exportFormatted(hs.endTime());
        eventObj[endKey] = endObj;
    }
    setHolyServiceGuidToRemoteMessage(hs, eventObj);

    //qDebug() << "JSON: " << QJsonDocument(eventObj).toJson();

    OutgoingEventRequest queuedReq;
    queuedReq.m_request = req;
    queuedReq.m_holyServiceGuid = hs.getId().toString();
    queuedReq.m_postedData = QJsonDocument(eventObj).toJson();
    queuedReq.m_operation = QNetworkAccessManager::PostOperation;

    m_outEventRequests.append(queuedReq);
}

void GoogleCalendarWizard::updateRemoteEvent(
    const RemoteEvent &remote, const HolyService &hs, bool setServiceGuid)
{
    const auto church = hs.getChurch();
    if (! church) return;

    const auto churchGuid = church->getId().toString();
    const auto calendarId = m_churchToCalendarId.value(churchGuid);

    if (calendarId.isEmpty()) return;

    auto req = jsonRequest(QString("https://www.googleapis.com/calendar/v3/calendars/%1/events/%2").
                                      arg(calendarId, remote.m_eventId), accessTokenQuery());
    QJsonObject eventObj;

    if (LocalAndRemote::summaryLine(hs) != remote.m_summary)
    {
        eventObj[summaryKey] = LocalAndRemote::summaryLine(hs);
    }
    if (remote.m_description != hs.intention())
    {
        eventObj[descriptionKey] = LocalAndRemote::descriptionLine(hs);
    }
    if (hs.startTime() != remote.m_start)
    {
        QJsonObject startObj;
        startObj[dateTimeKey] = exportFormatted(hs.startTime());
        eventObj[startKey] = startObj;
    }
    if (hs.endTime() != remote.m_end)
    {
        QJsonObject endObj;
        endObj[dateTimeKey] = exportFormatted(hs.endTime());
        eventObj[endKey] = endObj;
    }
    if (setServiceGuid)
    {
        setHolyServiceGuidToRemoteMessage(hs, eventObj);
    }

    if (eventObj.isEmpty()) return; // no change

    qDebug() << "UPDATE JSON: " << QJsonDocument(eventObj).toJson();

    OutgoingEventRequest queuedReq;
    queuedReq.m_request = req;
    queuedReq.m_holyServiceGuid = hs.getId().toString();
    queuedReq.m_operation = QNetworkAccessManager::CustomOperation;
    queuedReq.m_postedData = QJsonDocument(eventObj).toJson();
    queuedReq.m_postedDataBuffer = std::make_shared<QBuffer>();
    queuedReq.m_postedDataBuffer->setData(queuedReq.m_postedData);
    queuedReq.m_postedDataBuffer->open(QIODevice::ReadOnly);

    m_outEventRequests.append(queuedReq);
}

void GoogleCalendarWizard::removeRemoteEvent(const RemoteEvent &remote)
{
    auto req = jsonRequest(QString("https://www.googleapis.com/calendar/v3/calendars/%1/events/%2").
        arg(remote.m_calendarId, remote.m_eventId), accessTokenQuery());

    OutgoingEventRequest queuedReq;
    queuedReq.m_request = req;
    queuedReq.m_holyServiceGuid = remote.m_holyServiceGuid;
    queuedReq.m_operation = QNetworkAccessManager::DeleteOperation;

    m_outEventRequests.append(queuedReq);
}

QString GoogleCalendarWizard::exportFormatted(const QDateTime &dt)
{
    QDateTime dtCopy(dt);
    const auto offsetSecs = dtCopy.offsetFromUtc();
    dtCopy.setOffsetFromUtc(offsetSecs);
    return dtCopy.toString(Qt::ISODate);
}

QDateTime GoogleCalendarWizard::parseDtInJsonSubKey(const QJsonValue &dtValue)
{
    return parseDtString(dtValue.toObject().value(dateTimeKey).toString());
}

QDateTime GoogleCalendarWizard::parseDtString(const QString &dtIsoString)
{
    return QDateTime::fromString(dtIsoString, Qt::ISODate);
}

QNetworkRequest GoogleCalendarWizard::jsonRequest(const QString& bareUrl, const QUrlQuery &query)
{
    QUrl url(bareUrl);
    url.setQuery(query);

    QNetworkRequest retVal(url);
    retVal.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    return retVal;
}

QUrlQuery GoogleCalendarWizard::accessTokenQuery() const
{
    QUrlQuery query;
    query.addQueryItem("access_token", m_accessToken);
    return query;
}

bool GoogleCalendarWizard::isAccessTokenValidNow() const
{
    return (! m_accessToken.isEmpty()) && m_accessTokenValidTo.isValid() &&
            (QDateTime::currentDateTime().secsTo(m_accessTokenValidTo) > 10);
}

void GoogleCalendarWizard::setAccessTokenRelatedWidgets()
{
    const auto accessOk = isAccessTokenValidNow();

    m_ui->googleMailEdit->setEnabled(! accessOk);
    m_ui->authenticationPort->setEnabled(! accessOk);

    m_ui->tokenValidityLabel->setVisible(accessOk);
    m_ui->tokenValidityEdit->setVisible(accessOk);
    m_ui->accessTokenResetBtn->setVisible(! m_accessToken.isEmpty());
}

void GoogleCalendarWizard::on_accessTokenResetBtn_clicked()
{
    m_accessToken.clear();
    m_accessTokenValidTo = QDateTime();
    m_phase = INITIAL_STATE;

    setAccessTokenRelatedWidgets();

    m_ui->googleMailEdit->setFocus();
}

void GoogleCalendarWizard::resizeEvent(QResizeEvent *e)
{
    super::resizeEvent(e);

    // on Windows resize event comes before everything from m_ui is initialized
    if (m_phase == INITIAL_STATE) return;

    rebalanceTableColumns();
}

void GoogleCalendarWizard::rebalanceTableColumns()
{
    rebalanceOneTable(*m_ui->pc2webTable);
    rebalanceOneTable(*m_ui->web2pcTable);
    rebalanceOneTable(*m_ui->autoSynchroTable);
}

void GoogleCalendarWizard::rebalanceOneTable(QTableView& view)
{
    view.setColumnWidth(0, view.width() / 2);
    view.resizeRowsToContents();
}

void GoogleCalendarWizard::setDefaultPriestId(const IdTag &defaultPriestId)
{
    m_defaultPriestId = defaultPriestId;
}

void GoogleCalendarWizard::setDefaultChurch(const RemoteEvent& remote, HolyService &hs) const
{
    if (hs.hasChurch()) return;

    const auto churchId = m_churchToCalendarId.key(remote.m_calendarId);
    hs.setChurchId(IdTag::createFromString(churchId));
}

void GoogleCalendarWizard::setDefaultPriest(HolyService &hs) const
{
    if (! hs.hasPriest() && m_defaultPriestId.isValid())
    {
        hs.setPriestId(m_defaultPriestId);
    }
}

void GoogleCalendarWizard::updateLocalServices()
{
    m_localServices = HolyServiceIndexing::allSet().
            query(m_ui->fromDt->date(), m_ui->toDt->date());
    qDebug() << "Fetched " << m_localServices.size() << " local services between " <<
                m_ui->fromDt->date() << " and " << m_ui->toDt->date();
}

void GoogleCalendarWizard::on_startAutoSynchronization_clicked()
{
    QVector<LocalAndRemote> localNew, modified, remoteNew, remoteOrphaned;
    scanDifferences(localNew, modified, remoteNew, remoteOrphaned);

    const auto differing = localNew + modified + remoteNew + remoteOrphaned;

    if (differing.isEmpty())
    {
        QMessageBox::information(this, tr("Information"),
            tr("Sorry there is no known difference between remote and local data.\n\n"
               "Start wizard again if you want to have most fresh information."));
        return;
    }

    m_ui->autoSynchroProgress->setMaximum(differing.size());
    m_ui->autoSynchroProgress->setValue(0);

    for (const auto& diff : modified)
    {
        const auto localModifiedUtc = diff.m_local->lastModified().toUTC();
        if (localModifiedUtc < diff.m_remote.m_recentlyModified)
        {
            const auto userIntervened = transferDataFromRemote(diff.m_remote, *diff.m_local);
            if (userIntervened == USER_CHANGED_LOCAL_SERVICE)
            {
                updateRemoteEvent(diff.m_remote, *diff.m_local);
            }
            advanceProgressBar(*m_ui->autoSynchroProgress);
        }
        else
        {
            updateRemoteEvent(diff.m_remote, *diff.m_local);
        }
    }

    for (const auto& diff : remoteNew)
    {
        createNewLocalBasedOnRemote(diff.m_remote);
        advanceProgressBar(*m_ui->autoSynchroProgress);
    }

    for (const auto& diff : remoteOrphaned)
    {
        removeRemoteEvent(diff.m_remote);
    }

    for(const auto& diff : localNew)
    {
        createRemoteEvent(*diff.m_local);
    }

    startSendingOutgoingMessages();
}
