#include "GeneratorRunCommand.h"

#include "HolyServiceIndexing.h"

GeneratorRunCommand::GeneratorRunCommand(QUndoCommand* p) : super(p)
{
}

GeneratorRunCommand::~GeneratorRunCommand()
{
    foreach (const GeneratorRecord& r, m_generatorRunData)
    {
        delete r.m_old;
        delete r.m_new;
    }
}

void GeneratorRunCommand::registerSmoothlyAdded(const HolyService& newService)
{
    GeneratorRecord r;
    r.m_new = new HolyService;
    newService.putCopyInto(*r.m_new);
    r.m_id = newService.getId();
    m_generatorRunData.append(r);
}

void GeneratorRunCommand::registerReplaced(const HolyService& replaced, const HolyService& replacing)
{
    GeneratorRecord r;
    r.m_old = new HolyService;
    r.m_new = new HolyService;
    replaced.putCopyInto(*r.m_old);
    replacing.putCopyInto(*r.m_new);
    r.m_id = replaced.getId();

    m_generatorRunData.append(r);
}

void GeneratorRunCommand::redo()
{
    foreach (const GeneratorRecord& r, m_generatorRunData)
    {
        if (r.isPlainAddition())
        {
            HolyService* redoneService = new HolyService;
            redoneService->setId(r.m_id);
            
            HolyService* pConflicting = nullptr;
            HolyServiceIndexing::allSet().addElement(redoneService, pConflicting);
        }
        else
        {
            HolyService* respectiveService = HolyServiceIndexing::allSet().find(r.m_id);
            r.m_new->putCopyInto(*respectiveService);
        }
    }
}

void GeneratorRunCommand::undo()
{
    foreach (const GeneratorRecord& r, m_generatorRunData)
    {
        HolyService* respectiveService = HolyServiceIndexing::allSet().find(r.m_id);
        if (respectiveService == nullptr)
        {
            continue; // for any reason the service is no longer in allset - deleted or something
        }
        if (r.isPlainAddition())
        {
            HolyServiceIndexing::allSet().deleteElement(respectiveService);
        }
        else
        {
            r.m_old->putCopyInto(*respectiveService);
        }
    }
}
//------------------------------------------------------------------------------
GeneratorRunCommand::GeneratorRecord::GeneratorRecord()
{
    m_old = m_new = nullptr;
}

bool GeneratorRunCommand::GeneratorRecord::isPlainAddition() const
{
    return (m_old == nullptr);
}

