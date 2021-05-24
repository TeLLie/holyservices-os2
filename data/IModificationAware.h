#pragma once

class IModificationAware
{
public:
	virtual bool isDirty() const = 0;
	virtual void setDirty() = 0;
	virtual void setClean() = 0;

protected:
	IModificationAware(void) { }
	virtual ~IModificationAware(void) { }
};
