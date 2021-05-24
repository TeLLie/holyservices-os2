#-------------------------------------------------
#
# Project created by QtCreator 2012-02-10T17:18:05
#
#-------------------------------------------------

QT       += xml testlib

QT       += gui

TARGET = tst_xmlstoragetesttest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


INCLUDEPATH += ../../persistence \
    ../../persistence/xml \
    ../../data

SOURCES += tst_xmlstoragetesttest.cpp \
    ../../persistence/PersistentStoreFactory.cpp \
    ../../persistence/xml/XmlPersistentStore.cpp \
    ../../persistence/CurrentPersistentStore.cpp \
    ../../persistence/xml/PriestStorage.cpp \
    ../../persistence/xml/PersonStorage.cpp \
    ../../persistence/xml/ChurchStorage.cpp \
    ../../persistence/xml/HolyServiceStorage.cpp \
    ../../persistence/xml/DomStorageQueue.cpp \
    ../../persistence/xml/DomSaverThread.cpp \
    ../../persistence/xml/ClientStorage.cpp \
    ../../persistence/xml/BasicXmlTransformer.cpp \
    ../../common/IdTag.cpp \
    ../../data/Priest.cpp \
    ../../data/Person.cpp \
    ../../data/Church.cpp \
    ../../data/HolyService.cpp \
    ../../persistence/xml/ArchivingTask.cpp \
    ../../data/Client.cpp \
    ../../data/BasicModificationAware.cpp \
    ../../data/Identifiable.cpp \
    ../../common/HistoryData.cpp \
    ../../data/HolyServiceIndexing.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../persistence/PersistentStoreFactory.h \
    ../../persistence/IPersistentStore.h \
    ../../persistence/xml/XmlPersistentStore.h \
    ../../persistence/CurrentPersistentStore.h \
    ../../persistence/xml/PriestStorage.h \
    ../../persistence/xml/PersonStorage.h \
    ../../persistence/xml/ChurchStorage.h \
    ../../persistence/xml/HolyServiceStorage.h \
    ../../persistence/xml/DomStorageQueue.h \
    ../../persistence/xml/DomSaverThread.h \
    ../../persistence/xml/ClientStorage.h \
    ../../persistence/xml/BasicXmlTransformer.h \
    ../../common/IdTag.h \
    ../../data/Priest.h \
    ../../data/Person.h \
    ../../data/Church.h \
    ../../data/HolyServiceIndexing.h


	
