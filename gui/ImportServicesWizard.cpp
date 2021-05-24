#include "ImportServicesWizard.h"
#include "ui_ImportServicesWizard.h"

#include "../data/HolyServiceIndexing.h"
#include "../data/ChurchIndexing.h"
#include "../data/ClientIndexing.h"
#include "../data/PriestIndexing.h"

#include "UserSettings.h"
#include "WaitingCursor.h"

#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QRegularExpression>
#include <QtDebug>
#include <memory>

const QChar ImportServicesWizard::csvSeparator(';');

ImportServicesWizard::ImportServicesWizard(QWidget *parent) :
    QWizard(parent),
    m_ui(new Ui::ImportServicesWizard)
{
    m_ui->setupUi(this);

    {
        QPixmap churchColorDot(16, 16);
        const tChurchVector churches = ChurchIndexing::allSet().data();

        for(const Church* pChurch : churches)
        {
            churchColorDot.fill(pChurch->color());
            m_ui->churchCombo->addItem(churchColorDot, pChurch->toString(), QVariant(pChurch->getId().toString()));
            if (pChurch == UserSettings::get().defaultChurch())
            {
                m_ui->churchCombo->setCurrentIndex(m_ui->churchCombo->count() - 1);
            }
        }
    }
    {
        tPriestVector priests = PriestIndexing::allSet().data();
        Priest* pPriest;

        foreach(pPriest, priests)
        {
            m_ui->priestCombo->addItem(pPriest->toString(), QVariant(pPriest->getId().toString()));
            if (pPriest == UserSettings::get().defaultPriest())
            {
                m_ui->priestCombo->setCurrentIndex(m_ui->priestCombo->count() - 1);
            }
        }
    }

    for (const auto client : ClientIndexing::allSet().data())
    {
        m_ui->clientCombo->addItem(
                    client->toString(Client::SurnameNameStreet), client->getId().toString());
    }

    button(QWizard::NextButton)->setEnabled(false);

    connect(this, &QWizard::currentIdChanged,
            this, &ImportServicesWizard::onCurrentIdChanged);
}

ImportServicesWizard::~ImportServicesWizard()
{
    delete m_ui;
}

void ImportServicesWizard::setYear(int year)
{
    m_ui->yearBox->setValue(year);
}

void ImportServicesWizard::on_importedFileEdit_editingFinished()
{
    fillPreview();
}

void ImportServicesWizard::on_browseImportFileBtn_clicked()
{
    auto importedFile = QFileDialog::getOpenFileName(
                this, tr("Imported file"), QDir::homePath(),
                tr("CSV files (*.csv);;All files (*.*)"));
    if (importedFile.isEmpty())
        return;

    importedFile = QDir::toNativeSeparators(importedFile);
    m_ui->importedFileEdit->setText(importedFile);
    fillPreview();
}

void ImportServicesWizard::fillPreview()
{
    const auto sandHours = WaitingCursor{};

    button(QWizard::NextButton)->setEnabled(false);
    m_ui->filePreview->clear();
    m_ui->filePreview->setRowCount(0);
    m_ui->filePreview->setColumnCount(0);

    QFile f(m_ui->importedFileEdit->text());
    if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream ts(&f);
    m_csvLines.clear();
    ts.setCodec("UTF-8");
    while(! ts.atEnd())
    {
        m_csvLines.append(ts.readLine());
    }

    if (m_csvLines.isEmpty())
        return;

    const auto headerLine = m_csvLines.takeFirst();

    {
        const auto headerCells = headerLine.split(csvSeparator, QString::KeepEmptyParts);
        m_ui->filePreview->setRowCount(m_csvLines.size());
        m_ui->filePreview->setColumnCount(headerCells.size());
        m_ui->filePreview->setHorizontalHeaderLabels(headerCells);
    }

    for (auto row = 0; row < m_ui->filePreview->rowCount(); ++row)
    {
        const auto csvCells = m_csvLines.at(row).split(csvSeparator, QString::KeepEmptyParts);
        for (auto column = 0; column < m_ui->filePreview->columnCount(); ++column)
        {
            if (column >= csvCells.size())
                break;
            m_ui->filePreview->setItem(row, column, new QTableWidgetItem(csvCells.at(column)));
        }
    }
    m_ui->filePreview->resizeColumnsToContents();
    button(QWizard::NextButton)->setEnabled(true);
}

void ImportServicesWizard::onCurrentIdChanged(int id)
{
    if (id == RESULT_LOG_PAGE)
    {
        m_ui->plainTextEdit->clear();
        button(QWizard::CancelButton)->hide();

        auto church = getChurch();
        if (church == nullptr)
        {
            m_ui->plainTextEdit->appendPlainText(tr("No church was selected.\n"));
            return;
        }

        for (const auto& line : m_csvLines)
        {
            importOneService(line, church);
        }
    }
}

void ImportServicesWizard::importOneService(const QString& line, const Church* church)
{
    const auto csvCells = line.split(csvSeparator, QString::KeepEmptyParts);
    QDateTime start;
    int durationInMinutes = 45;
    QString intention;
    QString clientSurname, clientContact;
    IdTag priestId;

    switch(m_ui->filePreview->columnCount())
    {
    case 5:
        start = dateTime(csvCells.at(0), csvCells.at(1));
        intention = csvCells.at(2).simplified();
        clientSurname = csvCells.at(3).simplified();
        clientContact = csvCells.at(4).simplified();
        break;
    case 6: // skip 2nd column
        start = dateTime(csvCells.at(0), csvCells.at(2));
        intention = csvCells.at(3).simplified();
        clientSurname = csvCells.at(4).simplified();
        clientContact = csvCells.at(5).simplified();
        break;
    }

    // find out service length and priest from church detail data
    for (const auto &usualService : church->usualServices())
    {
        if ((Church::equivalent(usualService.m_serviceTerm) == start.date().dayOfWeek()) &&
                usualService.m_active)
        {
            durationInMinutes = usualService.m_durationInMinutes;
            priestId = usualService.m_priest;
            break;
        }
    }
    if (! priestId.isValid())
    {
        priestId = getPriestId();
    }

    // try to variate over available priests to resolve insertion conflict
    auto newService = std::make_unique<HolyService>();
    newService->setId(IdTag::createInitialized());
    newService->setChurch(church);
    newService->setPriestId(priestId);
    newService->setStartTime(start);
    newService->setEndTime(start.addSecs(durationInMinutes * 60));
    newService->setIntention(intention);

    QString warning;
    newService->setClientId(getClientId(clientSurname, clientContact, warning));
    if (! warning.isEmpty())
    {
        m_ui->plainTextEdit->appendPlainText(
                    tr("%1 (%2) - %3").
                        arg(start.toString(), intention, warning) + "\n");
    }

    HolyService* pConflicting = nullptr;
    if (HolyServiceIndexing::allSet().conflicts(*newService, pConflicting))
    {
        m_ui->plainTextEdit->appendPlainText(tr("%1 (%2) - time conflict, trying to switch to another priest").
                                             arg(start.toString(), intention) + "\n");
        for (const auto& priest : PriestIndexing::allSet().data())
        {
            newService->setPriest(priest);
            if (! HolyServiceIndexing::allSet().conflicts(*newService, pConflicting))
            {
                break;
            }
        }
    }

    if (pConflicting)
    {
        m_ui->plainTextEdit->appendPlainText(tr("%1 (%2) - time conflict could not be resolved, skipping this entry").
                                             arg(start.toString(), intention) + "\n");
    }
    else
    {
        HolyServiceIndexing::allSet().addElement(newService.release(), pConflicting);
    }
}

const Church *ImportServicesWizard::getChurch() const
{
    const int churchIndex = m_ui->churchCombo->currentIndex();
    QVariant data = m_ui->churchCombo->itemData(churchIndex);

    if (data.type() != QVariant::String)
        return nullptr;

    const auto churchId = IdTag::createFromString(data.toString());
    return ChurchIndexing::allSet().find(churchId);
}

IdTag ImportServicesWizard::getPriestId() const
{
    const int priestIndex = m_ui->priestCombo->currentIndex();
    QVariant data = m_ui->priestCombo->itemData(priestIndex);

    if (data.type() != QVariant::String)
        return {};

    return IdTag::createFromString(data.toString());
}

IdTag ImportServicesWizard::getClientId(const QString &surname, const QString &contact, QString& warning) const
{
    warning.clear();
    for (const auto& client : ClientIndexing::allSet().data())
    {
        const auto surnameFits = (client->surname().simplified() == surname);
        const auto contactFits = contact.isEmpty() ? true : (client->contact().simplified() == contact);
        if (surnameFits && contactFits)
        {
            return client->getId();
        }
    }

    const int clientIndex = m_ui->clientCombo->currentIndex();
    QVariant data = m_ui->clientCombo->itemData(clientIndex);

    if (data.type() != QVariant::String)
    {
        warning = tr("Client could not be found.");
        return {};
    }

    warning = tr("Client could not be found using surname %1 and contact %2, default client used instead.").
            arg(surname, contact);
    return IdTag::createFromString(data.toString());
}

QDateTime ImportServicesWizard::dateTime(const QString &dateStr, const QString &timeStr) const
{
    // dot separated day.month, optionally a year
    QRegularExpression dateRe("(\\d+)\\s*\\.\\s*(\\d+)\\s*(\\.\\s*\\d+)?");
    const auto m = dateRe.match(dateStr);
    QDate aDate;
    if (m.hasMatch())
    {
        auto parts = m.capturedTexts();
        const auto day = parts.at(1).toInt();
        const auto month = parts.at(2).toInt();
        auto year = m_ui->yearBox->value();
        if (parts.size() > 3)
        {
            auto yearStr = parts.at(3);
            yearStr = yearStr.mid(1).trimmed(); // skip the dot, remove spaces
            year = yearStr.toInt();
        }
        aDate.setDate(year, month, day);
        if (! aDate.isValid())
            return {};
    }
    const auto aTime = QTime::fromString(timeStr, "h:mm");
    if (! aTime.isValid())
        return {};
    return {aDate, aTime};
}

