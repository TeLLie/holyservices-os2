/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ServiceGeneratorDlg.h"
#include "ui_ServiceGeneratorDlg.h"

#include "GeneratorRunCommand.h"
#include "ButtonDecorator.h"
#include "ClientModel.h"
#include "PriestModel.h"
#include "ChurchModel.h"

#include "ClientIndexing.h"
#include "PriestIndexing.h"
#include "ChurchIndexing.h"
#include "HolyServiceIndexing.h"

#include "Client.h"
#include "Priest.h"
#include "Church.h"

#include "ServiceGenerator.h"

#include <QtDebug>
#include <QMessageBox>
#include <QButtonGroup>

ServiceGeneratorDlg::ServiceGeneratorDlg(QWidget* parent, const int year) : super(parent)
{
    m_ui = new Ui_ServiceGeneratorDlg;
    m_ui->setupUi(this);

    QStringList weekDays;

    for (int day = 1; day <= 7; ++day)
    {
        weekDays += QDate::longDayName(day);
    }
    m_ui->weekDayCombo1->addItems(weekDays);
    m_ui->weekDayCombo2->addItems(weekDays);

    {
        const QDate newYear(year, 1, 1);
        const QDate silvestr(year, 12, 31);

        m_ui->dateFrom->setDate(newYear);
        m_ui->startOfCountingDate->setDate(newYear);
        m_ui->dateTo->setDate(silvestr);
    }

    {
        const QTime t(17, 0);

        m_ui->timeFrom->setTime(t);
        m_ui->timeTo->setTime(t.addSecs(3600));
    }

    {
        QButtonGroup* pGroup = new QButtonGroup(this);

        pGroup->addButton(m_ui->everyNthWeek);
        pGroup->addButton(m_ui->nthWeekdayInMonth);
        pGroup->addButton(m_ui->usualServicesRadio);

        m_ui->everyNthWeek->toggle();
    }

    {
        QButtonGroup* pGroup = new QButtonGroup(this);

        pGroup->addButton(m_ui->skipCollisionRadio);
        pGroup->addButton(m_ui->replaceCollisionRadio);
    }

    m_undo = nullptr;
    updateUndoButtons();

    m_priestModel = new PriestModel(this);
    m_ui->priestCombo->setModel(m_priestModel);
    m_ui->priestCombo->setCurrentIndex(-1);

    m_clientModel = new ClientModel(this);
    m_ui->clientCombo->setModel(m_clientModel);
    m_ui->clientCombo->setCurrentIndex(-1);

    m_churchModel = new ChurchModel(this);
    m_ui->churchCombo->setModel(m_churchModel);

    connect(m_ui->generateButton, SIGNAL(clicked()), SLOT(generate()));
    connect(m_ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(close()));

    m_ui->usualChurchCombo->fillCombo();

    // when control at radio button is activated, switch radio
    connect(m_ui->usualChurchCombo, SIGNAL(activated(int)), m_ui->usualServicesRadio, SLOT(click()));
    connect(m_ui->everyNthSpin, SIGNAL(valueChanged(int)), m_ui->everyNthWeek, SLOT(click()));
    connect(m_ui->weekDayCombo1, SIGNAL(activated(int)), m_ui->everyNthWeek, SLOT(click()));
    connect(m_ui->startOfCountingDate, SIGNAL(dateChanged(const QDate&)), m_ui->everyNthWeek, SLOT(click()));
    connect(m_ui->weekdayOrder, SIGNAL(valueChanged(int)), m_ui->nthWeekdayInMonth, SLOT(click()));
    connect(m_ui->weekDayCombo2, SIGNAL(activated(int)), m_ui->nthWeekdayInMonth, SLOT(click()));

    // disable / enable stable controls when usual services options is selected
    // so as to give hint that stable controls do not apply for usual service generation
    connect(m_ui->usualServicesRadio, SIGNAL(toggled(bool)), SLOT(setStableControlsEnabled(bool)));

    connect(m_ui->undoButton, SIGNAL(clicked()), SLOT(undoRun()));
    connect(m_ui->redoButton, SIGNAL(clicked()), SLOT(redoRun()));

    ButtonDecorator::assignIcons(*m_ui->buttonBox);

    m_ui->progressBar->hide();
}

ServiceGeneratorDlg::~ServiceGeneratorDlg()
{
    delete m_ui;
}

void ServiceGeneratorDlg::generate()
{
    QString msg;
    const bool validity = checkValidity(msg);

    if (validity == false)
    {
        QMessageBox::warning(this, tr("Warning"), msg);
        return;
    }

    const bool suspicious = checkSuspiciousness(msg);
    if (suspicious)
    {
        const int answer = QMessageBox::question(this,
                                                 tr("Suspicious input"),
                                                 tr("Suspicious input:\n%1\n\nContinue in generating?").arg(msg),
                                                 QMessageBox::Yes | QMessageBox::No);

        if (answer != QMessageBox::Yes)
        {
            return;
        }
    }

    Church* pChurch = nullptr;
    Client* pClient = nullptr;
    Priest* pPriest = nullptr;

    // handle church input
    // select different stable church if the usual services for church option is selected
    if (m_ui->usualServicesRadio->isChecked())
    {
        pChurch = ChurchIndexing::allSet().find(m_ui->usualChurchCombo->currentId());
    }
    else
    {
        const QString strId = m_ui->churchCombo->itemData(m_ui->churchCombo->currentIndex()).toString();

        if (! strId.isEmpty())
        {
            const IdTag churchId = IdTag::createFromString(strId);

            pChurch = ChurchIndexing::allSet().find(churchId);
        }
    }

    // priest input
    {
        const QString strId = m_ui->priestCombo->itemData(m_ui->priestCombo->currentIndex()).toString();

        if (! strId.isEmpty())
        {
            const IdTag priestId = IdTag::createFromString(strId);

            pPriest = PriestIndexing::allSet().find(priestId);
        }
    }

    // client input
    {
        const QString strId = m_ui->clientCombo->itemData(m_ui->clientCombo->currentIndex()).toString();

        if (! strId.isEmpty())
        {
            const IdTag clientId = IdTag::createFromString(strId);

            pClient = ClientIndexing::allSet().find(clientId);
        }
    }

    // set up the generator
    ServiceGenerator generator;

    // by stable settings common to all generator services
    generator.setStableData(pChurch, pPriest, pClient,
                            m_ui->timeFrom->time(), m_ui->timeTo->time(),
                            m_ui->dateFrom->date(), m_ui->dateTo->date(),
                            m_ui->intentionEdit->text());

    tHolyServiceVector generated;

    if (m_ui->nthWeekdayInMonth->isChecked())
    {
        generated = generator.generateOneWeekdayInMonth(m_ui->weekdayOrder->value(),
                                                        static_cast<Qt::DayOfWeek>(m_ui->weekDayCombo2->currentIndex() + 1));
    }
    else if (m_ui->everyNthWeek->isChecked())
    {
        generated = generator.generateEveryWeekInMonth(m_ui->everyNthSpin->value(),
                                                       static_cast<Qt::DayOfWeek>(m_ui->weekDayCombo1->currentIndex() + 1),
                                                       m_ui->startOfCountingDate->date());
    }
    else if (m_ui->usualServicesRadio->isChecked())
    {
        generated = generator.generateForStableChurch();
    }

    ECollisionBehavior behavior = UndefinedBehavior;

    if (m_ui->skipCollisionRadio->isChecked())
    {
        behavior = SkipCollision;
    }
    else if (m_ui->replaceCollisionRadio->isChecked())
    {
        behavior = ReplaceCollision;
    }

    transferToAllSet(generated, behavior);
}

void ServiceGeneratorDlg::transferToAllSet(const tHolyServiceVector& services,
                                           const ServiceGeneratorDlg::ECollisionBehavior behavior)
{
    if (services.isEmpty())
    {
        return;
    }

    if (behavior == UndefinedBehavior)
    {
        assert(0);
        return ;
    }

    assert(m_undo != nullptr);
    GeneratorRunCommand* thisRunUndo = new GeneratorRunCommand;

    HolyService *pConflicting;

    m_ui->progressBar->show();
    m_ui->progressBar->setRange(0, services.size());
    int added = 0, replaced = 0, skipped = 0;
    int progress = 0;

    foreach (HolyService *p, services)
    {
        m_ui->progressBar->setValue(++progress);

        if (HolyServiceIndexing::allSet().addElement(p, pConflicting))
        {
            thisRunUndo->registerSmoothlyAdded(*p);
            ++added;
        }
        else
        {
            switch (behavior)
            {
            case SkipCollision:
                qDebug() << "New service " << p->toString() <<
                            " conflicts with " << pConflicting->toString() <<" it was SKIPPED";
                ++skipped;
                break;
            case ReplaceCollision:
                thisRunUndo->registerReplaced(*pConflicting, *p);
                qDebug() << "New service " << p->toString() <<
                            " conflicts with " << pConflicting->toString() <<" it was REPLACED";
                pConflicting->swap(*p);
                ++replaced;
                break;
            default:
                Q_ASSERT(0);
            }

            // prevent memory leaking of not added service
            delete p;
        }
    }

    thisRunUndo->setText(tr("Run %1, %2 added, %3 skipped, %4 replaced").
                         arg(QTime::currentTime().toString()).arg(added).arg(skipped).arg(replaced));

    m_undo->push(thisRunUndo);
    updateUndoButtons();
}

bool ServiceGeneratorDlg::checkSuspiciousness(QString& validationMsg) const
{
    bool retVal = false;
    validationMsg.clear();

    // check length of holy service, if it is not more than 3 hours
    const int serviceLength = m_ui->timeFrom->time().secsTo(m_ui->timeTo->time());

    if (serviceLength > 3 * 3600)
    {
        validationMsg += tr("Holy service is going to last over 3 hours (%1).").
                arg(serviceLength / 3600.0, 0, 'f', 1);
        retVal = true;
    }
    return retVal;
}

bool ServiceGeneratorDlg::checkValidity(QString& validationMsg) const
{
    bool retVal = true;
    validationMsg.clear();

    if (m_ui->dateFrom->date() >= m_ui->dateTo->date())
    {
        retVal = false;
        validationMsg += tr("\nStarting date must precede the ending date.");
    }
    if (m_ui->timeFrom->time() >= m_ui->timeTo->time())
    {
        retVal = false;
        validationMsg += tr("\nThe holy service time is incorrect - end comes before start.");
    }

    if (m_ui->everyNthWeek->isChecked())
    {
        if (m_ui->startOfCountingDate->date() > m_ui->dateTo->date())
        {
            retVal = false;
            validationMsg += tr("\nThe date the counting start from must precede the end date of generator interval."
                                "\n%1 comes after %2!").
                    arg(QLocale().toString(m_ui->startOfCountingDate->date()),
                        QLocale().toString(m_ui->dateTo->date()));
        }
    }

    if (retVal == false)
    {
        validationMsg += tr("\nGenerator will not be started because of invalid input.");
    }

    return retVal;
}

void ServiceGeneratorDlg::setStableControlsEnabled( bool usualServicesActive )
{
    QWidgetList commonControls = QWidgetList() <<
                                                  m_ui->timeFrom << m_ui->timeTo << m_ui->churchCombo << m_ui->priestCombo << m_ui->clientCombo <<
                                                  m_ui->intentionEdit;

    foreach(QWidget* w, commonControls)
    {
        w->setEnabled(! usualServicesActive);
    }
}

void ServiceGeneratorDlg::setUndo(QUndoStack* val)
{
    m_undo = val;
    updateUndoButtons();
}

void ServiceGeneratorDlg::updateUndoButtons()
{
    if (m_undo)
    {
        m_ui->undoButton->setText(tr("Undo (%1)").arg(m_undo->undoText()));
        m_ui->redoButton->setText(tr("Redo (%1)").arg(m_undo->redoText()));
    }
    m_ui->undoButton->setEnabled(m_undo && m_undo->canUndo());
    m_ui->redoButton->setEnabled(m_undo && m_undo->canRedo());

    const bool shouldBeVisible = (m_undo != nullptr) && (m_undo->canUndo() || m_undo->canRedo());

    m_ui->undoButton->setVisible(shouldBeVisible);
    m_ui->redoButton->setVisible(shouldBeVisible);
}

void ServiceGeneratorDlg::undoRun()
{
    m_undo->undo();
    updateUndoButtons();
}

void ServiceGeneratorDlg::redoRun()
{
    m_undo->redo();
    updateUndoButtons();
}
