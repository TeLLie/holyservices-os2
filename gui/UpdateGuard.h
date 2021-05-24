#ifndef UPDATEGUARD_H_INCLUDED_55944740_4D74_4018_BA10_9B40EBB17136
#define UPDATEGUARD_H_INCLUDED_55944740_4D74_4018_BA10_9B40EBB17136

///\class UpdateGuard
/// Simple guarding class that inhibits update of widget on its startup 
/// and enables the updates again on its destruction
///\date 3-2008
///\author Jan 'Kovis' Struhar

class QWidget;

class UpdateGuard
{
public:
    UpdateGuard(QWidget*);
    virtual ~UpdateGuard();

private:
	QWidget* m_widget;
};

#endif
