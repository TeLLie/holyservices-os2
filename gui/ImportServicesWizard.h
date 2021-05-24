#pragma once

#include <QWizard>

class Church;
class IdTag;
namespace Ui {
    class ImportServicesWizard;
}

class ImportServicesWizard : public QWizard
{
    Q_OBJECT

public:
    explicit ImportServicesWizard(QWidget *parent = nullptr);
    ~ImportServicesWizard();

    void setYear(int year);

private slots:
    void on_importedFileEdit_editingFinished();
    void on_browseImportFileBtn_clicked();
    void onCurrentIdChanged(int id);

private:
    Ui::ImportServicesWizard *m_ui = nullptr;
    static const QChar csvSeparator;
    QStringList m_csvLines;

    enum WizardPage
    {
        CSV_FILE_PAGE = 0,
        IMPORT_INFORMATION_PAGE,
        RESULT_LOG_PAGE
    };

    void fillPreview();
    QDateTime dateTime(const QString& dateStr, const QString& timeStr) const;
    const Church* getChurch() const;
    IdTag getPriestId() const;
    IdTag getClientId(const QString& surname, const QString &contact, QString &warning) const;
    void importOneService(const QString &line, const Church *church);
};

