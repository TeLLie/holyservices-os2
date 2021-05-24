#include "PersistentStoreFactory.h"
#include "IPersistentStore.h"
#include "HolyService.h"
#include "Client.h"
#include "Priest.h"
#include "Church.h"
#include "HolyServiceIndexing.h"
#include "ClientIndexing.h"

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

#include <memory>

class XmlStorageTestTest : public QObject
{
    Q_OBJECT
    
public:
    XmlStorageTestTest();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testServiceStoring();
    void testClientStoring();
    void testPriestStoring();
    void testChurchStoring();

private:
    void switchDatesOfFirstTwoRecords(const QString& storeFileName);
    void switchDates(HolyService& s1, HolyService& s2);
};

XmlStorageTestTest::XmlStorageTestTest()
{
}

void XmlStorageTestTest::initTestCase()
{
}

void XmlStorageTestTest::cleanupTestCase()
{
}

void XmlStorageTestTest::testServiceStoring()
{
    const QString storeFileName("testedServices.xml");

    QFile::remove(storeFileName);

    {
        std::auto_ptr<IPersistentStore> pStore(
                    PersistentStoreFactory::createPersistentStore(
                        PersistentStoreFactory::XML_DOM_STORAGE, storeFileName));
        QVERIFY2(pStore.get() != NULL, "Failure");

        std::auto_ptr<HolyService> h1(new HolyService);

        h1->setIntention("1111111111");
        h1->setStartTime(QDateTime(QDate(2012, 1, 1), QTime(15, 0)));
        h1->setEndTime(QDateTime(QDate(2012, 1, 1), QTime(16, 0)));
        h1->setId(IdTag::createInitialized());

        pStore->storeHolyService(h1.get());

        std::auto_ptr<HolyService> confa(new HolyService);

        confa->setIntention("2222222222");
        confa->setStartTime(QDateTime(QDate(2012, 8, 1), QTime(15, 0)));
        confa->setEndTime(QDateTime(QDate(2012, 8, 1), QTime(16, 0)));
        confa->setId(IdTag::createInitialized());

        pStore->storeHolyService(confa.get());

        pStore->close();
    }

    for (int testPass = 0; testPass < 10; ++testPass)
    {
        switchDatesOfFirstTwoRecords(storeFileName);

        {
            std::auto_ptr<IPersistentStore> pStore(
                        PersistentStoreFactory::createPersistentStore(
                            PersistentStoreFactory::XML_DOM_STORAGE, storeFileName));

            IPersistentStore::tHolyServiceList services;

            pStore->loadAllHolyServices(services);
            QVERIFY(services.size() == 2);

            HolyServiceIndexing::allSet().addElements(services);

            HolyService* s1 = HolyServiceIndexing::allSet().data().at(0);
            HolyService* s2 = HolyServiceIndexing::allSet().data().at(1);

            if (testPass % 2)
            {
                QVERIFY((s1->startTime().date().month() == 1) && s1->intention().startsWith("111"));
                QVERIFY((s2->startTime().date().month() == 8) && s2->intention().startsWith("222"));
            }
            else
            {
                QVERIFY((s1->startTime().date().month() == 1) && s1->intention().startsWith("222"));
                QVERIFY((s2->startTime().date().month() == 8) && s2->intention().startsWith("111"));
            }

            HolyServiceIndexing::releaseAllSet();
            pStore->close();
        }
    }

    // now switch them to and back
    {
        std::auto_ptr<IPersistentStore> pStore(
                    PersistentStoreFactory::createPersistentStore(
                        PersistentStoreFactory::XML_DOM_STORAGE, storeFileName));

        IPersistentStore::tHolyServiceList services;

        pStore->loadAllHolyServices(services);
        QVERIFY(services.size() == 2);

        HolyServiceIndexing::allSet().addElements(services);

        HolyService* s1 = HolyServiceIndexing::allSet().data().at(0);
        HolyService* s2 = HolyServiceIndexing::allSet().data().at(1);

        for (int testPass = 0; testPass < 20; ++testPass)
        {
            switchDates(*s1, *s2);
            switchDates(*s1, *s2);
            if (testPass % 2)
            {
                QVERIFY((s1->startTime().date().month() == 1) && s1->intention().startsWith("111"));
                QVERIFY((s2->startTime().date().month() == 8) && s2->intention().startsWith("222"));
            }
        }

        HolyServiceIndexing::releaseAllSet();
        pStore->close();
    }

    // now switch-store-switch-store-reload
    {
        std::auto_ptr<IPersistentStore> pStore(
                    PersistentStoreFactory::createPersistentStore(
                        PersistentStoreFactory::XML_DOM_STORAGE, storeFileName));

        IPersistentStore::tHolyServiceList services;

        pStore->loadAllHolyServices(services);
        QVERIFY(services.size() == 2);

        HolyServiceIndexing::allSet().addElements(services);

        HolyService* s1 = HolyServiceIndexing::allSet().data().at(0);
        HolyService* s2 = HolyServiceIndexing::allSet().data().at(1);

        // switch-store 1
        switchDates(*s1, *s2);

        pStore->storeHolyService(s1);
        pStore->storeHolyService(s2);

        // switch-store 2 used to lead to a failure - no changes saved
        switchDates(*s1, *s2);

        pStore->storeHolyService(s1);
        pStore->storeHolyService(s2);

        HolyServiceIndexing::releaseAllSet();
        pStore->close();
    }
    { // reload and verify that second switch restored original order
        std::auto_ptr<IPersistentStore> pStore(
                    PersistentStoreFactory::createPersistentStore(
                        PersistentStoreFactory::XML_DOM_STORAGE, storeFileName));

        IPersistentStore::tHolyServiceList services;

        pStore->loadAllHolyServices(services);
        QVERIFY(services.size() == 2);

        HolyServiceIndexing::allSet().addElements(services);

        HolyService* s1 = HolyServiceIndexing::allSet().data().at(0);
        HolyService* s2 = HolyServiceIndexing::allSet().data().at(1);

        QVERIFY((s1->startTime().date().month() == 1) && s1->intention().startsWith("111"));
        QVERIFY((s2->startTime().date().month() == 8) && s2->intention().startsWith("222"));

        HolyServiceIndexing::releaseAllSet();
        pStore->close();
    }
}

void XmlStorageTestTest::switchDatesOfFirstTwoRecords(const QString& storeFileName)
{
    std::auto_ptr<IPersistentStore> pStore(
                PersistentStoreFactory::createPersistentStore(
                    PersistentStoreFactory::XML_DOM_STORAGE, storeFileName));

    IPersistentStore::tHolyServiceList services;

    pStore->loadAllHolyServices(services);
    QVERIFY(services.size() == 2);

    HolyServiceIndexing::allSet().addElements(services);

    HolyService* s1 = HolyServiceIndexing::allSet().data().at(0);
    HolyService* s2 = HolyServiceIndexing::allSet().data().at(1);

    switchDates(*s1, *s2);

    pStore->storeHolyService(s1);
    pStore->storeHolyService(s2);

    QTest::qWait(500);

    HolyServiceIndexing::releaseAllSet();
    pStore->close();
}

void XmlStorageTestTest::switchDates(HolyService& s1, HolyService& s2)
{
    const QDateTime dtTempStart = s1.startTime();
    const QDateTime dtTempEnd = s1.endTime();

    s1.setStartTime(s2.startTime());
    s1.setEndTime(s2.endTime());

    s2.setStartTime(dtTempStart);
    s2.setEndTime(dtTempEnd);
}

void XmlStorageTestTest::testClientStoring()
{    const QString storeFileName("testedClients.xml");

     QFile::remove(storeFileName);

      const QString clientFirstName = QString::fromUtf8("Jo\xc5\xbeka"); // Joz^ka
      const QString clientFamilyName = QString::fromUtf8("Lampa");
        {
           std::auto_ptr<IPersistentStore> pStore(
                       PersistentStoreFactory::createPersistentStore(
                           PersistentStoreFactory::XML_DOM_STORAGE, storeFileName));
           QVERIFY2(pStore.get() != NULL, "Failure");

           std::auto_ptr<HolyService> h1(new HolyService);

           h1->setIntention("1111111111");
           h1->setStartTime(QDateTime(QDate(2012, 1, 1), QTime(15, 0)));
           h1->setEndTime(QDateTime(QDate(2012, 1, 1), QTime(16, 0)));
           h1->setId(IdTag::createInitialized());
           HolyService* conflict;
           HolyServiceIndexing::allSet().addElement(h1.get(), conflict);

           pStore->storeHolyService(h1.get());

           std::auto_ptr<Client> c1(new Client);
           c1->setId(IdTag::createInitialized());
           pStore->storeClient(c1.get());
           c1->setFirstName(clientFirstName);
           pStore->storeClient(c1.get());
           c1->setSurname(clientFamilyName);
           ClientIndexing::allSet().addElement(c1.get());

           pStore->storeClient(c1.get());

           h1->setClient(c1.get());
           pStore->storeHolyService(h1.get());

           HolyServiceIndexing::releaseAllSet();
           ClientIndexing::releaseAllSet();
           pStore->close();
        }
        {
           std::auto_ptr<IPersistentStore> pStore(
                       PersistentStoreFactory::createPersistentStore(
                           PersistentStoreFactory::XML_DOM_STORAGE, storeFileName));

           IPersistentStore::tHolyServiceList services;
           pStore->loadAllHolyServices(services);
           QVERIFY(services.size() == 1);
           HolyServiceIndexing::allSet().addElements(services);

           IPersistentStore::tClientList clients;
           pStore->loadAllClients(clients);
           QVERIFY(clients.size() == 1);
           ClientIndexing::allSet().addElements(clients);

           Client* c1 = clients.first();
           QVERIFY(c1->firstName() == clientFirstName);
           QVERIFY(c1->surname() == clientFamilyName);

           QVERIFY(services.first()->getClient() != NULL);

           HolyServiceIndexing::releaseAllSet();
           ClientIndexing::releaseAllSet();
           pStore->close();
        }


}

void XmlStorageTestTest::testPriestStoring()
{

}

void XmlStorageTestTest::testChurchStoring()
{

}

QTEST_MAIN(XmlStorageTestTest)

#include "tst_xmlstoragetesttest.moc"
