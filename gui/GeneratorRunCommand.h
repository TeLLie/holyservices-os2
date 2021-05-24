#ifndef GENERATORRUNCOMMAND_H_INCLUDED_061BD2AD_11F5_4790_BB11_83AE9458B301
#define GENERATORRUNCOMMAND_H_INCLUDED_061BD2AD_11F5_4790_BB11_83AE9458B301

///\class GeneratorRunCommand
///\brief Undo/redo command for generator runs
/// The class keeps record of generated services by value
///\date 10-2011
///\author Jan 'Kovis' Struhar jstruhar@amberg.cz
#include "../data/HolyService.h"
#include <QUndoCommand>
#include <QList>

class GeneratorRunCommand : public QUndoCommand
{
	typedef QUndoCommand super;
public:
        GeneratorRunCommand(QUndoCommand* p = 0);
	virtual ~GeneratorRunCommand();

        /// not conflicting adding
        void registerSmoothlyAdded(const HolyService& newService);

        /// case when generator replaces the original service -
        /// we will keep value copies - keep in mind that generator replaces via swap() call
        /// that keeps the identity - so the replaced ID is still valid key to the service
        void registerReplaced(const HolyService& replaced, const HolyService& replacing);

        virtual void redo();
        virtual void undo();
private:
    struct GeneratorRecord
    {
        GeneratorRecord();

        HolyService* m_old;
        HolyService* m_new;
        IdTag m_id;

        bool isPlainAddition() const;
    };

    QList<GeneratorRecord> m_generatorRunData;
};

#endif

