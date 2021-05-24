#pragma once
#include "IClassStorage.h"
#include "IPersistentStore.h"
#include <QString>
#include <assert.h>
#include <stddef.h>

///\class BasicClassStorage
///\brief Quiet template class already implementing holding of a
/// persistent store pointer and some suggestions for common data element 
/// attributes. 
/// Infrastructure naming 'root element' shifts the interface specialisation to XML
///\date 8-2006
///\author Jan 'Kovis' Struhar

template<typename T> class BasicClassStorage : public IClassStorage<T>
{
public:
    virtual ~BasicClassStorage(void) {}

    //@{
    ///\name Attribute (column) name suggestions
    static const QString idAttributeName; ///< record ID 
    /// superelement containing all record data, relevant for XML storage
    static const QString dataElementName; 
    /// record history data
    static const QString historyElementName; 
    //@}

    virtual bool setToBeStored(const T* const p)
    {
        if (m_persistentStore)
        {
            m_saved = p;
        }
        return (m_persistentStore != nullptr);
    }

    virtual bool getLoaded(T*& p) 
    {
        p = m_loaded;

        m_wasPickedUp = (m_loaded != nullptr);

        return m_wasPickedUp;
    }

    virtual void setStore(IPersistentStore *pStore)
    {
        assert(m_persistentStore == nullptr);

        m_persistentStore = pStore;
    }

    virtual IPersistentStore* getStore(void) const
    {
        return m_persistentStore;
    }

protected:
    //@{
    ///\name Attribute (column) name suggestions
    static const QString recordCreatedAttributeName; ///< creation time
    static const QString recordCreatorAttributeName; ///< creating person
    static const QString recordModifiedAttributeName; ///< modification time
    static const QString recordModificatorAttributeName; ///< modifying person
    //@}

    const T* m_saved;
    T* m_loaded;
    bool m_wasPickedUp; ///< debugging flag controlling, if loaded isnatnce has been picked up

    BasicClassStorage(void) : m_saved(nullptr), m_loaded(nullptr), m_wasPickedUp(false), 
        m_persistentStore(nullptr) {}

    const QString& rootElementName() const
    {
        return m_rootElementTagName;
    }

    void setRootElementName(const QString& newValue)
    {
        assert(m_rootElementTagName.isEmpty());
        m_rootElementTagName = newValue;
    }


private:
    IPersistentStore *m_persistentStore;
    QString m_rootElementTagName;
};

template<typename T> const QString BasicClassStorage<T>::dataElementName("Data");
template<typename T> const QString BasicClassStorage<T>::historyElementName("RecordHistory");
template<typename T> const QString BasicClassStorage<T>::idAttributeName("AppSpecificId");
template<typename T> const QString BasicClassStorage<T>::recordCreatedAttributeName("RecordCreatedAt");
template<typename T> const QString BasicClassStorage<T>::recordModifiedAttributeName("RecordRecentlyModifiedAt");
template<typename T> const QString BasicClassStorage<T>::recordCreatorAttributeName("RecordCreatedBy");
template<typename T> const QString BasicClassStorage<T>::recordModificatorAttributeName("RecordRecentlyModifiedBy");
