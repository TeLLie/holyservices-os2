#ifndef CURRENTPERSISTENTSTORE_H_INCLUDED_687433D4_B239_48B6_A35B_42964C0AD906
#define CURRENTPERSISTENTSTORE_H_INCLUDED_687433D4_B239_48B6_A35B_42964C0AD906

class IPersistentStore;

///\class CurrentPersistentStore
/// Holds instance of persistent store used as main persistent store
/// - global instance in the HolyServices program
/// It is not responsible for held instance.
///\date 9-2006
///\author Jan 'Kovis' Struhar

class CurrentPersistentStore
{
public:
	static IPersistentStore *get();
	static void set(IPersistentStore *);

private:
	static IPersistentStore* m_store;

    CurrentPersistentStore();
    virtual ~CurrentPersistentStore();
};

#endif
