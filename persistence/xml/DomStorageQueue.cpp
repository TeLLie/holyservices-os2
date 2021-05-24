/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "DomStorageQueue.h"

#include "../data/Identifiable.h"
#include "PriestStorage.h"
#include "ClientStorage.h"
#include "HolyServiceStorage.h"
#include "ChurchStorage.h"

#include <QMessageBox>
#include <QtDebug>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QTimer>

#include <assert.h>
#include <stdexcept>

QString DomStorageQueue::rootTag() { return "OAMDG";} // Omnes Ad Maiorem Dei Gloriam
QString DomStorageQueue::priestsTag() { return "Priests";}
QString DomStorageQueue::clientsTag() { return "ServiceClients";}
QString DomStorageQueue::holyServicesTag() { return "HolyServices";}
QString DomStorageQueue::churchesTag() { return "Churches";}

DomStorageQueue::DomStorageQueue(void)
{
    connect(&m_savingThread, SIGNAL(errorMessage(QString)), 
        SLOT(throwErrorMessagesAsRuntimeExceptions(QString)));
    connect(&m_savingThread, SIGNAL(finished()), 
        SLOT(savingFinished()));
}

DomStorageQueue::~DomStorageQueue(void)
{
    if (m_savingThread.isRunning())
    {
        m_savingThread.wait(10000);
    }
}

bool DomStorageQueue::init(const QString& fileName)
{
    bool retVal = false;
    QFile xmlFile(fileName);

    assert(m_incomingQueue.isEmpty() && m_savedQueue.isEmpty());

    m_xmlFileName = fileName;

    if (xmlFile.open(QIODevice::ReadOnly))
    {
        // load from file
        QString errorMsg;
        int errorLine, errorColumn;

        retVal = m_doc.setContent(&xmlFile, &errorMsg, &errorLine, &errorColumn);
        if (retVal)
        {
            // and validate that it is ours!
            QDomElement root = m_doc.documentElement();

            // check all mandatory data
            retVal = (root.tagName() == rootTag()) && 
                (! root.firstChildElement(priestsTag()).isNull()) &&
                (! root.firstChildElement(clientsTag()).isNull()) &&
                (! root.firstChildElement(churchesTag()).isNull()) &&
                (! root.firstChildElement(holyServicesTag()).isNull());

            if (retVal)
            {
                clearHelperStructures();
                loadHelperStructures();
                updateRememberedFileSize();
            }
            else
            {
                m_doc.clear();

                QMessageBox::warning(nullptr, QObject::tr("Warning"),
                    tr("File %1\nseems not to contain valid XML data for this program.").
                    arg(fileName));
            }
        }
        else
        {
            QMessageBox::warning(nullptr, QObject::tr("Warning"),
                tr("Error %1\nin the file %2\non line %3, column %4").
                arg(errorMsg, fileName).arg(errorLine).arg(errorColumn));
        }
    }
    else
    { // init initial memory structure
        QDomNode encodingPi = m_doc.createProcessingInstruction("xml", 
            "version=\"1.0\" encoding=\"UTF-8\"");
        m_doc.appendChild(encodingPi);

        QDomElement rootElem = m_doc.createElement(rootTag());
        // 1.0 was in the first version and lasted until 1.0.0.15
        // when the church structure has been extended by color and time presets
        rootElem.setAttribute("version", "1.1");
        m_doc.appendChild(rootElem);

        m_priestsElem = m_doc.createElement(priestsTag());
        rootElem.appendChild(m_priestsElem);

        m_servicesElem = m_doc.createElement(holyServicesTag());
        rootElem.appendChild(m_servicesElem);

        m_clientsElem = m_doc.createElement(clientsTag());
        rootElem.appendChild(m_clientsElem);

        m_churchesElem = m_doc.createElement(churchesTag());
        rootElem.appendChild(m_churchesElem);

        retVal = true;
    }
    return retVal;
}

void DomStorageQueue::enqueue(const EDataType dataType, const EOperation operation,
                              QDomNode* pNode, const IIdentifiable* const pIdentifiable)
{
    if ((dataType == UNKNOWN_DATATYPE) || (operation == UNKNOWN_OPERATION) || 
        (pIdentifiable == nullptr))
    {
        throw std::logic_error("Incorrect data for saving");
    }

    StorageRecord sr;

    sr.m_dataType = dataType;
    sr.m_operation = operation;
    sr.m_node = pNode;
    sr.m_id = pIdentifiable->getId();

//    qDebug() << "Queued for save record of type " << dataType << " ID: " << pIdentifiable->getId().toString();
//     const HolyService* hs = dynamic_cast<const HolyService*>(pIdentifiable);
//     if (hs)
//     {
//         qDebug() << "i.e. " << hs->toString();
// 
//         QString str;
//         QTextStream ts(&str);
//         ts << *pNode;
//         ts.flush();
//         qDebug() << "DOM wise " << str;
//     }

    m_incomingQueue.append(sr);

    processQueue();
}

bool DomStorageQueue::processQueue()
{
    assert(m_savedQueue.isEmpty());

    m_savedQueue += m_incomingQueue;
    m_incomingQueue.clear();

    StorageRecord sr;

    while (! m_savedQueue.isEmpty())
    {
        sr = m_savedQueue.takeFirst();

        switch (sr.m_operation)
        {
        case STORE:
            {
                QDomNode nodeFromQueue = sr.m_node->toDocument().documentElement();
                QDomNode copyOfQueued = m_doc.importNode(nodeFromQueue, true);
                const bool saveSuccess = saveToDom(sr, copyOfQueued);
                assert(saveSuccess);
            }
            break;
        case DELETE:
            deleteFromDom(sr);
            break;
        default:
            Q_ASSERT(0);
            break;
        }

        sr.releaseResources();
    }

    //	qDebug() << "Processqueue() " << m_doc.toString();

    delayedSave();

    return true;
}

bool DomStorageQueue::saveToDom(const StorageRecord& sr, const QDomNode& copyOfQueued)
{
    assert(sr.m_operation == STORE);

    bool retVal = true;

    switch(sr.m_dataType)
    {
    case HOLY_SERVICE:
        assert(! m_servicesElem.isNull());

        if (m_serviceMap.contains(sr.m_id))
        { // update DOM
            if (m_servicesElem.replaceChild(copyOfQueued, m_serviceMap[sr.m_id]).isNull())
            {
                throw "Error on service storage, exiting";
            }
            m_serviceMap[sr.m_id] = copyOfQueued;
        }
        else
        { // add
            m_serviceMap[sr.m_id] = m_servicesElem.appendChild(copyOfQueued);
        }
        break;
    case PRIEST:
        assert(! m_priestsElem.isNull());

        if (m_priestMap.contains(sr.m_id))
        { // update DOM
            if (m_priestsElem.replaceChild(copyOfQueued, m_priestMap[sr.m_id]).isNull())
            {
                throw "Error on priest storage, exiting";
            }
            m_priestMap[sr.m_id] = copyOfQueued;
        }
        else
        { // add
            m_priestMap[sr.m_id] = m_priestsElem.appendChild(copyOfQueued);
        }
        break;
    case CHURCH:
        assert(! m_churchesElem.isNull());

        if (m_churchMap.contains(sr.m_id))
        { // update DOM
            if (m_churchesElem.replaceChild(copyOfQueued, m_churchMap[sr.m_id]).isNull())
            {
                throw "Error on church storage, exiting";
            }
            m_churchMap[sr.m_id] = copyOfQueued;
        }
        else
        { // add
            m_churchMap[sr.m_id] = m_churchesElem.appendChild(copyOfQueued);
        }
        break;
    case CLIENT:
        assert(! m_clientsElem.isNull());

        if (m_clientMap.contains(sr.m_id))
        { // update DOM
            if (m_clientsElem.replaceChild(copyOfQueued, m_clientMap[sr.m_id]).isNull())
            {
                throw "Error on client storage, exiting";
            }
            m_clientMap[sr.m_id] = copyOfQueued;
        }
        else
        { // add
            m_clientMap[sr.m_id] = m_clientsElem.appendChild(copyOfQueued);
        }
        break;

    default:
        assert(0);
        retVal = false;
        break;
    }
    return retVal;
}

bool DomStorageQueue::deleteFromDom(const StorageRecord& sr)
{
    assert(sr.m_operation == DELETE);

    bool retVal = true;

    switch(sr.m_dataType)
    {
    case HOLY_SERVICE:
        retVal = (! m_servicesElem.isNull()) && m_serviceMap.contains(sr.m_id);
        assert(retVal);

        if (m_serviceMap.contains(sr.m_id))
        { // update DOM
            m_servicesElem.removeChild(m_serviceMap[sr.m_id]);
            m_serviceMap.remove(sr.m_id);
        }
        break;
    case PRIEST:
        retVal = (! m_priestsElem.isNull()) && m_priestMap.contains(sr.m_id);
        //		assert(retVal);

        if (m_priestMap.contains(sr.m_id))
        { // update DOM
            m_priestsElem.removeChild(m_priestMap[sr.m_id]);
            m_priestMap.remove(sr.m_id);
        }
        break;
    case CLIENT:
        retVal = (! m_clientsElem.isNull()) && m_clientMap.contains(sr.m_id);
        //		assert(retVal);

        if (m_clientMap.contains(sr.m_id))
        { // update DOM
            m_clientsElem.removeChild(m_clientMap[sr.m_id]);
            m_clientMap.remove(sr.m_id);
        }
        break;
    case CHURCH:
        retVal = (! m_churchesElem.isNull()) && m_churchMap.contains(sr.m_id);
        //		assert(retVal);

        if (m_churchMap.contains(sr.m_id))
        { // update DOM
            m_churchesElem.removeChild(m_churchMap[sr.m_id]);
            m_churchMap.remove(sr.m_id);
        }
        break;

    default:
        assert(0);
        retVal = false;
        break;
    }

    return retVal;
}

void DomStorageQueue::clearHelperStructures()
{
    assert(m_incomingQueue.isEmpty() && m_savedQueue.isEmpty());

    ///\todo DOM nodes held by queues will do memory leak now
    m_incomingQueue.clear();
    m_savedQueue.clear();

    m_priestsElem.clear();
    m_clientsElem.clear();
    m_servicesElem.clear();
    m_churchesElem.clear();

    m_priestMap.clear();
    m_clientMap.clear();
    m_serviceMap.clear();
    m_churchMap.clear();
}

void DomStorageQueue::loadHelperStructures()
{ // all m_doc sanity checks done in init()
    const QDomElement root = m_doc.documentElement();

    loadPriestStructures(root);
    loadClientStructures(root);
    loadServiceStructures(root);
    loadChurchStructures(root);
}

void DomStorageQueue::loadPriestStructures(const QDomElement& root)
{
    QDomElement aElement, dataElement;
    IdTag idTag;

    m_priestsElem = root.firstChildElement(priestsTag());
    for (aElement = m_priestsElem.firstChildElement(PriestStorage::priestTagName); 
        (! aElement.isNull()); 
        aElement = aElement.nextSiblingElement(PriestStorage::priestTagName))
    {
        dataElement = aElement.firstChildElement(PriestStorage::dataElementName);

        const bool elementValid = (! dataElement.isNull()) &&
            dataElement.hasAttribute(PriestStorage::idAttributeName) &&
            (! dataElement.attribute(PriestStorage::idAttributeName).isEmpty());

        assert(elementValid);

        if (elementValid)
        {
            idTag = IdTag::createFromString(dataElement.attribute(PriestStorage::idAttributeName));

            assert(idTag.isValid());

            m_priestMap[idTag] = aElement;
        }
    }
}

void DomStorageQueue::loadChurchStructures(const QDomElement& root)
{
    QDomElement aElement, dataElement;
    IdTag idTag;

    m_churchesElem = root.firstChildElement(churchesTag());
    for (aElement = m_churchesElem.firstChildElement(ChurchStorage::churchTagName); 
        (! aElement.isNull()); 
        aElement = aElement.nextSiblingElement(ChurchStorage::churchTagName))
    {
        dataElement = aElement.firstChildElement(ChurchStorage::dataElementName);

        const bool elementValid = (! dataElement.isNull()) &&
            dataElement.hasAttribute(ChurchStorage::idAttributeName) &&
            (! dataElement.attribute(ChurchStorage::idAttributeName).isEmpty());

        assert(elementValid);

        if (elementValid)
        {
            idTag = IdTag::createFromString(dataElement.attribute(ChurchStorage::idAttributeName));

            assert(idTag.isValid());

            m_churchMap[idTag] = aElement;
        }
    }
}

void DomStorageQueue::loadClientStructures(const QDomElement& root)
{
    QDomElement aElement, dataElement;
    IdTag idTag;

    m_clientsElem = root.firstChildElement(clientsTag());
    for (aElement = m_clientsElem.firstChildElement(ClientStorage::clientTagName); 
        (! aElement.isNull()); 
        aElement = aElement.nextSiblingElement(ClientStorage::clientTagName))
    {
        dataElement = aElement.firstChildElement(ClientStorage::dataElementName);

        const bool elementValid = (! dataElement.isNull()) &&
            dataElement.hasAttribute(ClientStorage::idAttributeName) &&
            (! dataElement.attribute(ClientStorage::idAttributeName).isEmpty());

        assert(elementValid);

        if (elementValid)
        {
            idTag = IdTag::createFromString(dataElement.attribute(ClientStorage::idAttributeName));

            assert(idTag.isValid());

            m_clientMap[idTag] = aElement;
        }
    }
}

void DomStorageQueue::loadServiceStructures(const QDomElement& root)
{
    QDomElement aElement, dataElement;
    IdTag idTag;

    m_servicesElem = root.firstChildElement(holyServicesTag());
    for (aElement = m_servicesElem.firstChildElement(HolyServiceStorage::holyServiceTagName); 
        (! aElement.isNull()); 
        aElement = aElement.nextSiblingElement(HolyServiceStorage::holyServiceTagName))
    {
        dataElement = aElement.firstChildElement(HolyServiceStorage::dataElementName);

        const bool elementValid = (! dataElement.isNull()) &&
            dataElement.hasAttribute(HolyServiceStorage::idAttributeName) &&
            (! dataElement.attribute(HolyServiceStorage::idAttributeName).isEmpty());

        assert(elementValid);

        if (elementValid)
        {
            idTag = IdTag::createFromString(dataElement.attribute(HolyServiceStorage::idAttributeName));

            assert(idTag.isValid());

            m_serviceMap[idTag] = aElement;
        }
    }
}

QDomNode DomStorageQueue::firstServiceNode()
{
    return m_servicesElem.firstChildElement(HolyServiceStorage::holyServiceTagName);
}

QDomNode DomStorageQueue::nextServiceNode(const QDomNode& fromNode)
{
    assert((! fromNode.isNull()) && fromNode.isElement());

    return fromNode.nextSiblingElement(HolyServiceStorage::holyServiceTagName);
}

QDomNode DomStorageQueue::firstClientNode()
{
    return m_clientsElem.firstChildElement(ClientStorage::clientTagName);
}

QDomNode DomStorageQueue::nextClientNode(const QDomNode& fromNode)
{
    assert((! fromNode.isNull()) && fromNode.isElement());

    return fromNode.nextSiblingElement(ClientStorage::clientTagName);
}

QDomNode DomStorageQueue::firstPriestNode()
{
    return m_priestsElem.firstChildElement(PriestStorage::priestTagName);
}

QDomNode DomStorageQueue::nextPriestNode(const QDomNode& fromNode)
{
    assert((! fromNode.isNull()) && fromNode.isElement());

    return fromNode.nextSiblingElement(PriestStorage::priestTagName);
}

QDomNode DomStorageQueue::firstChurchNode()
{
    return m_churchesElem.firstChildElement(ChurchStorage::churchTagName);
}

QDomNode DomStorageQueue::nextChurchNode(const QDomNode& fromNode)
{
    assert((! fromNode.isNull()) && fromNode.isElement());

    return fromNode.nextSiblingElement(ChurchStorage::churchTagName);
}

void DomStorageQueue::throwErrorMessagesAsRuntimeExceptions(QString msg)
{
    QByteArray msgUtf8 = msg.toUtf8();

    throw std::runtime_error(msgUtf8.constData());
}

void DomStorageQueue::delayedSave()
{
    if (! m_delayedSavingTriggered)
    {
        // leave a time space so that other writing requests can come, too
        QTimer::singleShot(1000, this, SLOT(saveImmediately()));
        m_delayedSavingTriggered = true;
    }
}

void DomStorageQueue::saveImmediately()
{
    saveImmediately(false);
}

void DomStorageQueue::saveImmediately(bool synchronnously)
{
    if (m_rememberedLastFileSize >= 0)
    {
        const auto fileSizeNow = QFileInfo(m_xmlFileName).size();
        if (fileSizeNow != m_rememberedLastFileSize)
        {
            m_delayedSavingTriggered = false;
            throwErrorMessagesAsRuntimeExceptions(
                        tr("Someone modified the file %1 concurrently with you.\n"
                           "Your changes will not be written and you do best if you close the application to prevent data loss.\n"
                           "Concurrent data writing is not supported.").arg(m_xmlFileName));
        }
    }

    if (m_savingThread.isRunning())
    {
        delayedSave();
        qDebug() << "Saving thread still running";
    }
    else
    {
//        qDebug() << "sent to saving thread() " << m_doc.toString();

        m_savingThread.setData(m_doc, m_xmlFileName);

//        qDebug() << "saving thread started";

        m_savingThread.start();

        if (synchronnously)
        {
            m_savingThread.wait();
        }
    }
}

void DomStorageQueue::savingFinished()
{
    Q_ASSERT(m_delayedSavingTriggered);
    m_delayedSavingTriggered = false;
    updateRememberedFileSize();
    //    qDebug() << "delayed save finished";
}

void DomStorageQueue::updateRememberedFileSize()
{
    m_rememberedLastFileSize = QFileInfo(m_xmlFileName).size();
}

/////////////////////////////////////////////
// inner class implementation
DomStorageQueue::StorageRecord::StorageRecord()
{
    m_dataType = UNKNOWN_DATATYPE;
    m_operation = UNKNOWN_OPERATION;
    m_node = nullptr;
}

void DomStorageQueue::StorageRecord::releaseResources()
{
    if (m_node)
    {
        delete m_node;
        m_node = nullptr;
    }
}
