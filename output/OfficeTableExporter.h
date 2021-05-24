#ifndef OFFICETABLEEXPORTER_H
#define OFFICETABLEEXPORTER_H

///\class OfficeTableExporter
/// Puts into clipboard html fragment that can be inserted
/// as table in MS Office / OpenOffice documents.
/// Basically a clipboard copy of HMl export
///\date 11-2010
///\author Jan 'Kovis' Struhar

#include "DocumentExporter.h"

class OfficeTableExporter : public DocumentExporter
{
    typedef DocumentExporter super;
public:
    OfficeTableExporter(const tColumnDescriptions&);
    virtual ~OfficeTableExporter();

    virtual void close();
};

#endif // OFFICETABLEEXPORTER_H
