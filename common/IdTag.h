#pragma once

#include <QString>
#include <QUuid>

///\class IdTag
///\brief Identification tag for various objects
/// Key for all persistency related things, reference between persisted data
///\author Jan 'Kovis' Struhar kovis@sourceforge.net
///\date 8-2006
class IdTag
{
public:
	static IdTag createInitialized();
	static IdTag createFromString(const QString& strId);

	IdTag(void); ///< tag is created as unititalized, empty
	IdTag(const IdTag&); 
    IdTag& operator=(const IdTag& rhs);

    void clear(void);
    void initId(); ///< create brand new ID

    void setIdFromString(const QString& newId);
    QString toString() const;

    bool isValid() const; ///< contains valid ID?

	bool operator<(const IdTag&) const;
	bool operator==(const IdTag&) const;
	bool operator!=(const IdTag&) const;

private:
	QUuid m_id;

    void swap(IdTag& other) noexcept;
};
