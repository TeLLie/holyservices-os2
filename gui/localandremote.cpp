#include "localandremote.h"

#include "UserSettings.h"
#include "HolyService.h"
#include "Client.h"

#include <QObject>

bool RemoteEvent::isValid() const
{
    return ! (m_eventId.isEmpty() || m_calendarId.isEmpty());
}

bool RemoteEvent::operator ==(const HolyService &hs) const
{
    const bool descAndTimeEquals =
            (m_description == LocalAndRemote::descriptionLine(hs))
            && (m_start == hs.startTime()) && (m_end == hs.endTime());
    if (! descAndTimeEquals) return false;

    bool clientEquals = false;
    if (UserSettings::get().isClientImportant())
    {
        if (hs.hasClient())
        {
            clientEquals = (LocalAndRemote::massClient(hs) == m_summary);
        }
        else
        {
            clientEquals = m_summary.trimmed().isEmpty();
        }
    }
    else
    {
        clientEquals = (hs.intention() == m_summary);
    }
    return clientEquals;
}

QString LocalAndRemote::summaryLine(const HolyService& hs)
{
    if (UserSettings::get().isClientImportant())
    {
        return massClient(hs);
    }
    return hs.intention();
}

QString LocalAndRemote::descriptionLine(const HolyService& hs)
{
    if (UserSettings::get().isClientImportant())
    {
        return hs.intention();
    }
    return massClient(hs);
}

QString LocalAndRemote::massClient(const HolyService &hs)
{
    if (hs.getClient())
    {
        return hs.getClient()->toString(Client::SurnameName).trimmed();
    }
    return {};
}

QString LocalAndRemote::localToRemoteDiff(const QString &prefix) const
{
    if (!(m_local && m_remote.isValid()))
    {
        return QString();
    }

    QString diff;

    if (UserSettings::get().isClientImportant())
    {
        addToDiff(diff, prefix, QObject::tr("Client"), summaryLine(*m_local), m_remote.m_summary);
        addToDiff(diff, prefix, QObject::tr("Intention"), descriptionLine(*m_local), m_remote.m_description);
    }
    else
    {
        addToDiff(diff, prefix, QObject::tr("Client"), descriptionLine(*m_local), m_remote.m_description);
        addToDiff(diff, prefix, QObject::tr("Intention"), summaryLine(*m_local), m_remote.m_summary);
    }
    addToDiff(diff, prefix, QObject::tr("Date"), m_local->startTime().toLocalTime().date().toString(Qt::DefaultLocaleShortDate),
              m_remote.m_start.toLocalTime().date().toString(Qt::DefaultLocaleShortDate));
    addToDiff(diff, prefix, QObject::tr("Time"), m_local->startTime().toLocalTime().time().toString(Qt::DefaultLocaleShortDate),
              m_remote.m_start.toLocalTime().time().toString(Qt::DefaultLocaleShortDate));
    return diff;
}

QString LocalAndRemote::remoteToLocalDiff(const QString &prefix) const
{
    if (m_local && m_remote.isValid())
    {
        QString diff;
        if (UserSettings::get().isClientImportant())
        {
            addToDiff(diff, prefix, QObject::tr("Client"), m_remote.m_summary, summaryLine(*m_local));
            addToDiff(diff, prefix, QObject::tr("Intention"), m_remote.m_description, descriptionLine(*m_local));
        }
        else
        {
            addToDiff(diff, prefix, QObject::tr("Client"), m_remote.m_description, descriptionLine(*m_local));
            addToDiff(diff, prefix, QObject::tr("Intention"), m_remote.m_summary, summaryLine(*m_local));
        }
        addToDiff(diff, prefix, QObject::tr("Date"), m_remote.m_start.toLocalTime().date().toString(Qt::DefaultLocaleShortDate),
                  m_local->startTime().toLocalTime().date().toString(Qt::DefaultLocaleShortDate));
        addToDiff(diff, prefix, QObject::tr("Time"), m_remote.m_start.toLocalTime().time().toString(Qt::DefaultLocaleShortDate),
                  m_local->startTime().toLocalTime().time().toString(Qt::DefaultLocaleShortDate));
        return diff;
    }
    return QString();
}

void LocalAndRemote::addToDiff(QString &diff, const QString& prefix, const QString &title,
                               const QString &expression1, const QString &expression2)
{
    if (expression1 != expression2)
    {
        diff += QString("%1%2: %3").arg(prefix, title, expression1);
    }
}

