#ifndef ARCHIVINGTASK_H
#define ARCHIVINGTASK_H

///\class ArchivingTask
/// zip and rename saved xml file in a separated task
/// do one copy immediately
///\author Jan 'Kovis' Struhar
///\date 11-2010

#include <QRunnable>
#include <QString>

class ArchivingTask : public QRunnable
{
public:
    ArchivingTask(const QString& fileNameToArchive);
    virtual ~ArchivingTask();

    static const QString archiveFileExtension;

    virtual void run();

private:
    QString m_fileNameToArchive;

    void backUpNow(const QString& archiveName);
};

#endif // ARCHIVINGTASK_H
