# enable new C++ standard features
QMAKE_CXXFLAGS += -std=c++14

TEMPLATE = app
TARGET = HolyServices
#CONFIG += release
QT += network widgets printsupport
#CONFIG += debug
#PRECOMPILED_HEADER = precompiled/precompiledHeaders.h
#PRECOMPILED_SOURCE = precompiled/precompiledHeaders.cpp
DEPENDPATH += . \
    celebrations \
    common \
    data \
    gui \
    holidays \
    output \
    persistence \
    precompiled \
    preferences \
    persistence/xml
INCLUDEPATH += . \
    common \
    data \
    persistence \
    gui \
    preferences \
    holidays \
    persistence/xml
DESTDIR = ./build/bin
MOC_DIR = ./build/moc
UI_DIR = ./build/ui
CONFIG(release):OBJECTS_DIR = ./build/release-obj
CONFIG(debug):OBJECTS_DIR = ./build/debug-obj

# avoid asserts in release mode for MinGW compiler
CONFIG(release):DEFINES += NDEBUG
RC_FILE = gui/winResources.rc
TRANSLATIONS = ./translations/cz/cs_CZ.ts \
    ./translations/untranslated/untranslated.ts \
    ./translations/sk/sk_SK.ts \
    ./translations/it/it_IT.ts \
    ./translations/pt/pt_PT.ts \
    ./translations/es/es_ES.ts \
    ./translations/de/de_DE.ts
QT += xml

# Input
HEADERS += celebrations/ICelebrations.h \
    common/GenericIndexing.h \
    common/HistoryData.h \
    common/IdTag.h \
    data/AndFilter.h \
    data/AssignedFilter.h \
    data/BasicModificationAware.h \
    data/Church.h \
    data/ChurchFilter.h \
    data/ChurchIndexing.h \
    data/ChurchModel.h \
    data/Client.h \
    data/ClientFilter.h \
    data/ClientIndexing.h \
    data/ClientModel.h \
    data/functors.h \
    data/HolyService.h \
    data/HolyServiceIndexing.h \
    data/HsTypedefs.h \
    data/Identifiable.h \
    data/IHistoryData.h \
    data/IIdentifiable.h \
    data/IModificationAware.h \
    data/ImportCsv.h \
    data/IServiceFilter.h \
    data/Person.h \
    data/Priest.h \
    data/PriestFilter.h \
    data/PriestIndexing.h \
    data/PriestModel.h \
    data/ServiceGenerator.h \
    gui/AboutDlg.h \
    gui/ButtonDecorator.h \
    gui/CalendarDelegate.h \
    gui/CalendarModel.h \
    gui/CalendarView.h \
    gui/CalendarWidget.h \
    gui/CalendarWidgetPrivate.h \
    gui/ChurchEditor.h \
    gui/ChurchWeekDayWidget.h \
    gui/ClientOverview.h \
    gui/ClientComboBox.h \
    gui/ColumnDescriptionsModel.h \
    gui/DateInputDlg.h \
    gui/FocusReportingEdit.h \
    gui/HolyServiceApp.h \
    gui/HolyServiceSelection.h \
    gui/HolyServicesMF.h \
    gui/IdComboBox.h \
    gui/ImportServicesWizard.h \
    gui/NewFileWizard.h \
    gui/OutputOptionsDlg.h \
	gui/PriestComboBox.h \
    gui/SelectFromListDlg.h \
    gui/ReportColumnConfigurationDlg.h \
    gui/ServiceCalendarWidget.h \
    gui/ServiceDetailDlg.h \
    gui/ServiceGeneratorDlg.h \
    gui/ServiceMonthWidget.h \
    gui/UpdateGuard.h \
    gui/UsualChurchCombo.h \
    gui/WaitingCursor.h \
    holidays/IHolidays.h \
    output/CalendarExporter.h \
    output/CsvExporter.h \
    output/DocumentExporter.h \
    output/HtmlExporter.h \
    output/IExporter.h \
    output/PrinterExporter.h \
    output/ReportColumnDescription.h \
    output/XmlExporter.h \
    persistence/BasicClassStorage.h \
    persistence/CurrentPersistentStore.h \
    persistence/IClassStorage.h \
    persistence/IPersistentStore.h \
    persistence/IXmlTransformer.h \
    persistence/PersistentStoreFactory.h \
    precompiled/precompiledHeaders.h \
    preferences/UserSettings.h \
    persistence/xml/BasicXmlTransformer.h \
    persistence/xml/ChurchStorage.h \
    persistence/xml/ClientStorage.h \
    persistence/xml/DomSaverThread.h \
    persistence/xml/DomStorageQueue.h \
    persistence/xml/HolyServiceStorage.h \
    persistence/xml/PersonStorage.h \
    persistence/xml/PriestStorage.h \
    persistence/xml/XmlPersistentStore.h \ 
    gui/ChurchWeekTemplateWidget.h \
    holidays/CommonChristianHolidays.h \
    holidays/LocalHolidays.h \
    holidays/HolidayComposition.h \
    persistence/HolidayStorage.h \
    holidays/HolidayRecord.h \
    output/OfficeTableExporter.h \
    output/OpenOfficeExporter.h \
    persistence/xml/ArchivingTask.h \
    gui/HolidayListModel.h \
    gui/HolidayManagementDlg.h \
    gui/GeneratorRunCommand.h \
    gui/AddSingleHolidayDlg.h \
    data/MovableServiceFilter.h \
    gui/CountMoneyContributionsDlg.h \
    data/daterangefilter.h \
    gui/GoogleCalendarWizard.h \
    gui/ServiceTimeShiftWizard.h \
    gui/localremotediffmodel.h \
    gui/localandremote.h \
    gui/ClientDetailDlg.h
FORMS += gui/AboutDlg.ui \
    gui/ClientOverview.ui \
    gui/ChurchEditor.ui \
    gui/DateInputDlg.ui \
    gui/HolyServiceSelection.ui \
    gui/HolyServicesMF.ui \
    gui/ImportServicesWizard.ui \
    gui/OutputOptionsDlg.ui \
    gui/ReportColumnConfigurationDlg.ui \
    gui/SelectFromListDlg.ui \
    gui/ServiceDetailDlg.ui \
    gui/ServiceGeneratorDlg.ui \
    gui/AddSingleHolidayDlg.ui \
    gui/CountMoneyContributionsDlg.ui \
    gui/ServiceTimeShiftWizard.ui \
    gui/GoogleCalendarWizard.ui \
    gui/ClientDetailDlg.ui
SOURCES += common/HistoryData.cpp \
    common/IdTag.cpp \
    data/AndFilter.cpp \
    data/AssignedFilter.cpp \
    data/BasicModificationAware.cpp \
    data/Church.cpp \
    data/ChurchFilter.cpp \
    data/ChurchIndexing.cpp \
    data/ChurchModel.cpp \
    data/Client.cpp \
    data/ClientFilter.cpp \
    data/ClientIndexing.cpp \
    data/ClientModel.cpp \
    data/HolyService.cpp \
    data/HolyServiceIndexing.cpp \
    data/Identifiable.cpp \
    data/ImportCsv.cpp \
    data/Person.cpp \
    data/Priest.cpp \
    data/PriestFilter.cpp \
    data/PriestIndexing.cpp \
    data/PriestModel.cpp \
    data/ServiceGenerator.cpp \
    gui/AboutDlg.cpp \
    gui/ButtonDecorator.cpp \
    gui/CalendarDelegate.cpp \
    gui/CalendarModel.cpp \
    gui/CalendarView.cpp \
    gui/CalendarWidget.cpp \
    gui/CalendarWidgetPrivate.cpp \
    gui/ChurchEditor.cpp \
    gui/ChurchWeekDayWidget.cpp \
    gui/ClientComboBox.cpp \
    gui/ClientOverview.cpp \
    gui/ColumnDescriptionsModel.cpp \
    gui/DateInputDlg.cpp \
    gui/FocusReportingEdit.cpp \
    gui/HolyServiceApp.cpp \
    gui/HolyServiceSelection.cpp \
    gui/HolyServicesMF.cpp \
    gui/IdComboBox.cpp \
    gui/ImportServicesWizard.cpp \
    gui/main.cpp \
    gui/NewFileWizard.cpp \
    gui/OutputOptionsDlg.cpp \
    gui/PriestComboBox.cpp \
    gui/ReportColumnConfigurationDlg.cpp \
    gui/SelectFromListDlg.cpp \
    gui/ServiceCalendarWidget.cpp \
    gui/ServiceDetailDlg.cpp \
    gui/ServiceGeneratorDlg.cpp \
    gui/ServiceMonthWidget.cpp \
    gui/UpdateGuard.cpp \
    gui/UsualChurchCombo.cpp \
    gui/WaitingCursor.cpp \
    output/CalendarExporter.cpp \
    output/CsvExporter.cpp \
    output/DocumentExporter.cpp \
    output/HtmlExporter.cpp \
    output/PrinterExporter.cpp \
    output/ReportColumnDescription.cpp \
    output/XmlExporter.cpp \
    persistence/CurrentPersistentStore.cpp \
    persistence/PersistentStoreFactory.cpp \
    preferences/UserSettings.cpp \
    persistence/xml/BasicXmlTransformer.cpp \
    persistence/xml/ChurchStorage.cpp \
    persistence/xml/ClientStorage.cpp \
    persistence/xml/DomSaverThread.cpp \
    persistence/xml/DomStorageQueue.cpp \
    persistence/xml/HolyServiceStorage.cpp \
    persistence/xml/PersonStorage.cpp \
    persistence/xml/PriestStorage.cpp \
    persistence/xml/XmlPersistentStore.cpp \ 
    gui/ChurchWeekTemplateWidget.cpp \
    holidays/CommonChristianHolidays.cpp \
    holidays/LocalHolidays.cpp \
    holidays/HolidayComposition.cpp \
    persistence/HolidayStorage.cpp \
    holidays/HolidayRecord.cpp \ 
    output/OfficeTableExporter.cpp \
    output/OpenOfficeExporter.cpp \
    persistence/xml/ArchivingTask.cpp \
    gui/HolidayListModel.cpp \
    gui/HolidayManagementDlg.cpp \
    gui/GeneratorRunCommand.cpp \
    gui/AddSingleHolidayDlg.cpp \
    data/MovableServiceFilter.cpp \
    gui/CountMoneyContributionsDlg.cpp \
    output/IExporter.cpp \
    data/daterangefilter.cpp \
    gui/GoogleCalendarWizard.cpp \
    gui/ServiceTimeShiftWizard.cpp \
    gui/localremotediffmodel.cpp \
    gui/localandremote.cpp \
    gui/ClientDetailDlg.cpp

RESOURCES += gui/HolyServices.qrc

#win32 {
# LIBS += advapi32.lib
#}














