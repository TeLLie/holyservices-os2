#ifndef OPENOFFICEEXPORTER_H
#define OPENOFFICEEXPORTER_H

///\class OfficeTableExporter
/// Open Document Format export
///\date 11-2010
///\author Jan 'Kovis' Struhar

#include "DocumentExporter.h"

class OpenOfficeExporter : public DocumentExporter
{
    typedef DocumentExporter super;
public:
    OpenOfficeExporter(const tColumnDescriptions&);
    virtual ~OpenOfficeExporter();

    virtual void close();
};

#endif // OPENOFFICEEXPORTER_H
