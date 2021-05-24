/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ArchivingTask.h"

#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QtDebug>

const QString ArchivingTask::archiveFileExtension("archivedHolyServices");

ArchivingTask::ArchivingTask(const QString& fileNameToArchive)
{
    m_fileNameToArchive = fileNameToArchive;
}

ArchivingTask::~ArchivingTask()
{
}

void ArchivingTask::run()
{
    const QFileInfo fi(m_fileNameToArchive);
    // create archive folder structure
    QDir fileDir = fi.dir();
    const QString archiveFolderName("archive");

    fileDir.mkdir(archiveFolderName);

    if (fileDir.cd(archiveFolderName))
    {
        // have a look if there is backup of the file not older than 1 week
        QString archiveMask = fi.completeBaseName();
        archiveMask += "_*.";
        archiveMask += archiveFileExtension;
        QStringList archiveNameFilterList;

        archiveNameFilterList << archiveMask;

        const QFileInfoList archiveList = fileDir.entryInfoList(archiveNameFilterList,
            QDir::Files | QDir::Readable, QDir::Time);
        const QString newArchiveName = fileDir.absoluteFilePath(QString("%1_%2.%3").
            arg(fi.completeBaseName(), QDate::currentDate().toString(Qt::ISODate), archiveFileExtension));

        if (archiveList.isEmpty())
        {
            backUpNow(newArchiveName);
            return;
        }

        // do nothing if last backup is fresh enough
        const int secondsInOneWeek = 7 * 86400;
#if QT_VERSION >= 0x051000
        const QDateTime newestArchiveCreated = archiveList.first().birthTime();
#else
        const QDateTime newestArchiveCreated = archiveList.first().lastModified();
#endif
        if (newestArchiveCreated.secsTo(QDateTime::currentDateTime()) >  secondsInOneWeek)
        {
            // create fresh compressed backup
            backUpNow(newArchiveName);
        }
    }
}

void ArchivingTask::backUpNow(const QString& archiveName)
{
    // read the source (conflicts with opened-for-write file not expected)
    QFile sourceFile(m_fileNameToArchive);

    if (! sourceFile.open(QIODevice::ReadOnly))
    {
        return;
    }

    QFile archiveFile(archiveName);
    if (archiveFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        const QByteArray sourceFileContents = sourceFile.readAll();

        if (! sourceFileContents.isEmpty())
        {
            // compress contents
            const QByteArray compressed = qCompress(sourceFileContents, 9);

            // write down the archive
            const qint64 bytesWritten = archiveFile.write(compressed);
            Q_ASSERT(bytesWritten == compressed.size());
            (void) bytesWritten; // do not complain abut unused variable in release
            qDebug() << "Compress ratio " << (100.0 * compressed.size()) / sourceFileContents.size() << "%";
        }
        archiveFile.close();
    }

    sourceFile.close(); // yes, destructor would close the file, too
}
