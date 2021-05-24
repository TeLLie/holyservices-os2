#-------------------------------------------------
#
# Project created by QtCreator 2010-11-12T07:45:06
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = HolidayStorageTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DESTDIR = ../../../build/bin
MOC_DIR = ../../../build/moc
UI_DIR = ../../../build/ui
CONFIG(release):OBJECTS_DIR = ../../../build/release-obj

# avoid asserts in release mode for MinGW compiler
CONFIG(release):DEFINES += NDEBUG
CONFIG(debug):OBJECTS_DIR = ../../../build/debug-obj


INCLUDEPATH += ../../persistence \
    ../../holidays

SOURCES += HolidayStorageTest.cpp \
    ../../holidays/HolidayRecord.cpp \
    ../../persistence/HolidayStorage.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../holidays/HolidayRecord.h \
    ../../persistence/HolidayStorage.h
