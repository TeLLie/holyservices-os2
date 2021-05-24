/*!
  \class LocalAndRemote
  \author Jan 'kovis' Struhar
*/
#ifndef LOCALANDREMOTE_H
#define LOCALANDREMOTE_H

#include <QString>
#include <QDateTime>

class HolyService;

struct RemoteEvent {
    QString m_eventId;
    QString m_calendarId;
    QString m_holyServiceGuid;
    QString m_summary;
    QString m_description;
    QDateTime m_start;
    QDateTime m_end;
    QDateTime m_recentlyModified;

    bool isValid() const;
    bool operator ==(const HolyService& hs) const;
};

struct LocalAndRemote {
    RemoteEvent m_remote;
    HolyService* m_local = nullptr;

    static QString summaryLine(const HolyService &hs);
    static QString descriptionLine(const HolyService &hs);

    QString localToRemoteDiff(const QString& prefix) const;
    QString remoteToLocalDiff(const QString& prefix) const;
    static void addToDiff(QString& diff, const QString &prefix, const QString& title,
                          const QString& expression1, const QString& expression2);
    static QString massClient(const HolyService& hs);
};


#endif // LOCALANDREMOTE_H
