#pragma once

#include "../common/IdTag.h"
#include "DomSaverThread.h"

#include <QObject>
#include <QMap>
#include <QList>
#include <QDomDocument>

class QDomNode;
class IIdentifiable;

///\class DomStorageQueue
///\brief Instance gathers requests for storage and the storing
/// happens in separate thread
///\date 8-2006
///\author Jan 'Kovis' Struhar

class DomStorageQueue : public QObject
{
	Q_OBJECT
public:
	enum EDataType { UNKNOWN_DATATYPE, PRIEST, HOLY_SERVICE, CLIENT, CHURCH };
	enum EOperation { UNKNOWN_OPERATION, STORE, DELETE };

    DomStorageQueue(void);
    virtual ~DomStorageQueue(void);

    bool init(const QString& fileName);

	void enqueue(const EDataType dataType, const EOperation operation,
		QDomNode* pNode, const IIdentifiable* const pIdentifiable);

	QDomNode firstServiceNode();
	QDomNode nextServiceNode(const QDomNode& fromNode);

	QDomNode firstClientNode();
	QDomNode nextClientNode(const QDomNode& fromNode);

	QDomNode firstPriestNode();
	QDomNode nextPriestNode(const QDomNode& fromNode);

	QDomNode firstChurchNode();
	QDomNode nextChurchNode(const QDomNode& fromNode);

public:
	void saveImmediately(bool synchronnously);

private slots:
	void saveImmediately();
	void throwErrorMessagesAsRuntimeExceptions(QString);
	void savingFinished();
    void updateRememberedFileSize();

private:
	//@{
	///\name Document tags
	static QString rootTag();
	static QString priestsTag();
	static QString holyServicesTag();
	static QString clientsTag();
	static QString churchesTag();
	//@}

	QString m_xmlFileName;
    /// safeguard for concurrent file modification
    qint64 m_rememberedLastFileSize = -1;
    bool m_delayedSavingTriggered = false;

	class StorageRecord
	{
	public:
		EDataType m_dataType;
		EOperation m_operation;
		QDomNode *m_node;
                IdTag m_id;

		StorageRecord();
		void releaseResources();
	};

	typedef QList<StorageRecord> tStorageList;
	typedef tStorageList::iterator tStorageListIt;

	DomSaverThread m_savingThread;

	tStorageList m_incomingQueue;
	tStorageList m_savedQueue;

	QDomDocument m_doc; ///< main data holding structure

	//@{
	///\name Shortcuts to main m_doc elements - pseudo "tables"
	QDomElement m_clientsElem;
	QDomElement m_priestsElem;
	QDomElement m_servicesElem;
	QDomElement m_churchesElem;
	//@}

	/// lookup acceleration
        typedef QMap<IdTag, QDomNode> tIdNodeMap;

	//@{
	///\name Primary indices according to ID
	tIdNodeMap m_priestMap;
	tIdNodeMap m_serviceMap;
	tIdNodeMap m_clientMap;
	tIdNodeMap m_churchMap;
	//@}

	bool processQueue();

	/// save queued objects into a XML file
	bool saveToDom(const StorageRecord& sr, const QDomNode& copyOfQueued);

	bool deleteFromDom(const StorageRecord& sr);

	/// empty all index maps and other helper structures
	void clearHelperStructures();

	/// reconstruct the helper structures from the DOM document
	void loadHelperStructures();

	void loadPriestStructures(const QDomElement& root);
	void loadClientStructures(const QDomElement& root);
	void loadServiceStructures(const QDomElement& root);
	void loadChurchStructures(const QDomElement& root);

	/// save all modifications gathered in incoming queue - 
	/// if we save modifications immediately, we would overwrite the file on every
	/// miserable record - if we delay save a bit we will save all at once
	void delayedSave();
};
