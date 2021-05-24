/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "HolidayStorage.h"
#include <QtCore/QString>
#include <QtTest/QtTest>

class HolidayStorageTest : public QObject
{
    Q_OBJECT

public:
    HolidayStorageTest();

private Q_SLOTS:
    void testStoring();
    void testLoading();

private:
    HolidayRecord r1, r2, r3;
};

HolidayStorageTest::HolidayStorageTest()
{
    r1.setDate(QDate(2008, 1, 1)); r1.setDescription("Novy Rok");
    r2.setDate(QDate(2009, 12, 7)); r2.setDescription("Pearl Harbor");
    r3.setDate(QDate(2009, 12, 24)); r3.setDescription("Vanoce");
}

void HolidayStorageTest::testStoring()
{
    QList<HolidayRecord> records;

    records << r1 << r2 << r3;

    QVERIFY2(HolidayStorage::storeToFile("testStoring1.txt", records), "Failure on storing holidayrecords");
}

void HolidayStorageTest::testLoading()
{
    const QString fileName("testLoading1.txt");
    {
        QList<HolidayRecord> records;

        records << r2 << r1 << r3;

        HolidayStorage::storeToFile(fileName, records);
    }

    QList<HolidayRecord> loadedData;
    QVERIFY2(HolidayStorage::loadFromFile(fileName, loadedData), "failed on loading");

    // data should be loaded sorted regardless of order in the file
    QVERIFY2(loadedData.at(0) == r1, "unsorted on 1st record");
    QVERIFY2(loadedData.at(1) == r2, "unsorted on 2nd record");
    QVERIFY2(loadedData.at(2) == r3, "unsorted on 3rd record");
}

QTEST_APPLESS_MAIN(HolidayStorageTest);

#include "HolidayStorageTest.moc"
