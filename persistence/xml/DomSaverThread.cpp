/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "DomSaverThread.h"

#include "ArchivingTask.h"

#include <QFile>
#include <QTextStream>
#include <QThreadPool>

#include <stdexcept>

DomSaverThread::DomSaverThread()
{
}

DomSaverThread::~DomSaverThread()
{
}

void DomSaverThread::setData(const QDomDocument& savedDocument, const QString& fileName)
{
    Q_ASSERT(! isRunning());

    if (! isRunning())
    {
        m_toBeSaved = savedDocument.cloneNode(true).toDocument();
        m_fileName = fileName;
    }
}

void DomSaverThread::run(void)
{
    const auto xmlText = m_toBeSaved.toString(2);

    QFile f(m_fileName);
    const auto retVal = f.open(QIODevice::WriteOnly);
    if (retVal)
    {
        QTextStream ts(&f);

        ts.setCodec("UTF-8");

        ts << xmlText;

        if (f.error() != QFile::NoError)
        {
            emit errorMessage(tr("Error writing file %1\n%2").
                arg(m_fileName, f.errorString()));
        }

        f.close();

        ArchivingTask* archiver = new ArchivingTask(m_fileName);
        QThreadPool::globalInstance()->start(archiver);
    }
    else
    {
        emit errorMessage(tr("File %1 cannot be opened for writing.\n"
            "Changes will not be saved.").
            arg(m_fileName));
    }
}
