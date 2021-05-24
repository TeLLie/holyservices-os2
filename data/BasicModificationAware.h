#pragma once
#include "IModificationAware.h"

///\class BasicModificationAware
/// Common implementation of IModificationAware interface
///\author Jan 'Kovis' Struhar
///\date 8/2006

class BasicModificationAware : public IModificationAware
{
public:
    BasicModificationAware(void);
    virtual ~BasicModificationAware(void);

	virtual bool isDirty() const;
	virtual void setDirty();
	virtual void setClean();

private:
	bool m_dirty;
};
