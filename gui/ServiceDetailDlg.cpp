/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ServiceDetailDlg.h"

#include "ui_ServiceDetailDlg.h"

#include "ChurchModel.h"
#include "PriestModel.h"
#include "ClientModel.h"

#include "HolyService.h"
#include "Church.h"
#include "Priest.h"
#include "Client.h"

#include "HolyServiceIndexing.h"
#include "ClientIndexing.h"
#include "ClientFilter.h"
#include "SelectFromListDlg.h"
#include "DateInputDlg.h"
#include "ClientDetailDlg.h"
#include "ButtonDecorator.h"
#include "IdComboBox.h"

#include <QMessageBox>
#include <QFocusEvent>
#include <QTableView>
#include <QHeaderView>
#include <QCompleter>
#include <QtDebug>
#include <locale.h>
#include <memory>

ServiceDetailDlg::ServiceDetailDlg(QWidget *parent) : QDialog(parent)
{
    m_ui = new Ui_ServiceDetailDlg;
    m_ui->setupUi(this);

	m_service = nullptr;

    m_churchModel = new ChurchModel(this);
    m_priestModel = new PriestModel(this);

    {
        ClientModel* m = new ClientModel(this);
        m->setReadOnly(true);
        m_clientModel = m;
    }

	{
        m_ui->churchCombo->setView(IdComboBox::createViewForComboPopup(QSize(450, 200)));
        m_ui->churchCombo->setModel(m_churchModel);
        QTableView* churchTable = dynamic_cast<QTableView*>(m_ui->churchCombo->view());
        if (churchTable)
        {
            churchTable->resizeColumnsToContents();
        }
    }
	{
        m_ui->priestCombo->setView(IdComboBox::createViewForComboPopup(QSize(250, 200)));
        m_ui->priestCombo->setModel(m_priestModel);
	}
	{
        m_ui->clientCombo->setView(IdComboBox::createViewForComboPopup(QSize(700, 350)));
        m_ui->clientCombo->setModel(m_clientModel);
        QTableView* clientTable = dynamic_cast<QTableView*>(m_ui->clientCombo->view());
        if (clientTable)
        {
            clientTable->resizeColumnsToContents();
        }
	}

    connect(m_ui->clearClientAssignmentButton, &QAbstractButton::clicked,
            this, &ServiceDetailDlg::clearClientAssignment);
    connect(m_ui->newClientBtn, &QAbstractButton::clicked, this, &ServiceDetailDlg::addNewClient);
    connect(m_ui->buttonBox, SIGNAL(accepted()), SLOT(okPressed()));
    connect(m_ui->changeDateButton, SIGNAL(clicked()), SLOT(changeServiceDate()));
    connect(m_ui->churchCombo, SIGNAL(currentIndexChanged(int)), SLOT(churchIndexChanged(int)));
    connect(m_ui->clientCombo, SIGNAL(currentIndexChanged(int)), SLOT(clientIndexChanged(int)));
    connect(m_ui->clientCombo->completer(), QOverload<const QModelIndex&>::of(&QCompleter::highlighted),
            this, &ServiceDetailDlg::selectHintedClient);
    connect(m_ui->intentionEdit, SIGNAL(acquiredFocus()), SLOT(intentionGotFocus()));
    connect(m_ui->historicalInfoButton, SIGNAL(clicked()), SLOT(showRecordHistory()));

    ButtonDecorator::assignIcons(*m_ui->buttonBox);
}

ServiceDetailDlg::~ServiceDetailDlg()
{
    delete m_ui;
}

void ServiceDetailDlg::okPressed()
{
	dialogToObject();

	if (serviceValidityCheck() == true)
	{
		accept();
	}
}

void ServiceDetailDlg::setService(HolyService *p)
{
	m_service = p;
    objectToDialog();
}

bool ServiceDetailDlg::serviceValidityCheck() const
{
	const bool serviceValid = m_service->isValid(); 
	
	if (! serviceValid)
	{
		QMessageBox::warning(parentWidget(), tr("Warning"), 
			tr("The holy service should be carefully checked\n"
			"if it is not too long and have priest and church assigned"));
	}
	return serviceValid;
}

void ServiceDetailDlg::objectToDialog()
{
	assert(m_service);

	if (m_service)
	{
		serviceValidityCheck();

        m_ui->dateDisplay->setText(m_service->startTime().date().toString(Qt::LocaleDate));

        m_ui->intentionEdit->setPlainText(m_service->intention());
        m_ui->fromTimeEdit->setTime(m_service->startTime().time());
        m_ui->endTimeEdit->setTime(m_service->endTime().time());
        m_ui->moneyBox->setValue(m_service->receivedMoney());
        m_ui->fixedToDateButon->setChecked(m_service->timeConstraint() == HolyService::FixedToDate);
        m_ui->movableButton->setChecked(m_service->timeConstraint() == HolyService::Movable);

        const bool intentionIsDefined = (! m_service->intention().trimmed().isEmpty());
        bool churchAssigned = true;
        bool priestAssigned = true;
        bool clientAssigned = true;
        {
            const Client * const serviceClient = m_service->getClient();
            selectServiceClient(serviceClient);
            if (serviceClient == nullptr)
            {
                clientAssigned = false;
                if (! m_newClientNameHint.isEmpty())
                {
                    m_ui->clientDescriptionLabel->setText(m_newClientNameHint);
                }
            }
        }
        {
            const Priest * const servicePriest = m_service->getPriest();
            const int priestIndex = (servicePriest) ?
                m_ui->priestCombo->findData(QVariant(servicePriest->getId().toString())) :
                -1;

            m_ui->priestCombo->setCurrentIndex(priestIndex);
            if (servicePriest == nullptr)
            {
                priestAssigned = false;
            }
        }
		{
            const Church * const serviceChurch = m_service->getChurch();
			const int churchIndex = (serviceChurch) ?
                m_ui->churchCombo->findData(QVariant(serviceChurch->getId().toString())) :
				-1;
			
            m_ui->churchCombo->setCurrentIndex(churchIndex);
			churchIndexChanged(churchIndex);
            if (serviceChurch == nullptr)
            {
                churchAssigned = false;
            }
		}

        // solve the smart transfer of focus
        if (priestAssigned && clientAssigned && churchAssigned)
        {
            if (intentionIsDefined)
            {
                m_ui->intentionEdit->setFocus();
            }
            else
            { // on empty intention it wants something before intention and clear
                // button is too dangerous
                m_ui->clientCombo->setFocus();
            }
        }
        else if (churchAssigned)
        {
            if (priestAssigned)
            {
                m_ui->clientCombo->setFocus();
            }
            else
            {
                m_ui->priestCombo->setFocus();
            }
        }
        else
        {
            m_ui->churchCombo->setFocus();
        }
	}
}

void ServiceDetailDlg::dialogToObject()
{
	assert(m_service);

	if (m_service)
	{
        m_service->setIntention(m_ui->intentionEdit->toPlainText());

		{
			QDateTime start = m_service->startTime();
            start.setTime(m_ui->fromTimeEdit->time());
			m_service->setStartTime(start);
		}
		{
			QDateTime end = m_service->endTime();
            end.setTime(m_ui->endTimeEdit->time());
			m_service->setEndTime(end);
		}
		{
            const QString strId = m_ui->churchCombo->itemData(m_ui->churchCombo->currentIndex()).toString();

			if (! strId.isEmpty())
			{
				const IdTag churchId = IdTag::createFromString(strId);

				m_service->setChurchId(churchId);
			}
		}
		{
            const QString strId = m_ui->priestCombo->itemData(m_ui->priestCombo->currentIndex()).toString();

			if (! strId.isEmpty())
			{
				const IdTag priestId = IdTag::createFromString(strId);

				m_service->setPriestId(priestId);
			}
		}
		{
            const QString strId = m_ui->clientCombo->itemData(m_ui->clientCombo->currentIndex()).toString();
			if (! strId.isEmpty())
			{
				const IdTag clientId = IdTag::createFromString(strId);

				m_service->setClientId(clientId);
			}
			else
			{
				m_service->setClientId(IdTag());
			}
		}
        {
            m_service->setReceivedMoney(m_ui->moneyBox->value());
        }
        {
            if (m_ui->fixedToDateButon->isChecked())
            {
                m_service->setTimeConstraint(HolyService::FixedToDate);
            }
            else
            {
                m_service->setTimeConstraint(HolyService::Movable);
            }
        }
	}
}

struct DateIntention
{
    QDate m_date;
    QString m_intention;

    bool operator<(const DateIntention& rhs) const 
    {
        return this->m_date > rhs.m_date;
    }
};

QString ServiceDetailDlg::choiceFromPreviousIntentions() const
{
    const QString stringId = m_ui->clientCombo->itemData(m_ui->clientCombo->currentIndex()).toString();
	const IdTag clientId = (stringId.isEmpty()) ? IdTag() : IdTag::createFromString(stringId);
    ClientFilter clientFilter(clientId);

    const tHolyServiceVector clientServices = HolyServiceIndexing::filter(HolyServiceIndexing::allSet().data(),
    &clientFilter);
    QMap<QString, QDate> intentionDuplicityEliminator;
    QVectorIterator<HolyService*> it(clientServices);

    while (it.hasNext())
    {
        const HolyService* p = it.next();
        intentionDuplicityEliminator[p->intention()] = p->startTime().date();
    }

    if (! intentionDuplicityEliminator.empty())
    {
        // sort by date descendingly, now it would be alphabetically on intention
        QMapIterator<QString, QDate> mapIt(intentionDuplicityEliminator);
        QVector<DateIntention> dateSorted;
        while (mapIt.hasNext())
        {
            mapIt.next();

            dateSorted.reserve(intentionDuplicityEliminator.size());
            DateIntention rec;

            rec.m_date = mapIt.value();
            rec.m_intention = mapIt.key();

            dateSorted.append(rec);
        }

        std::sort(dateSorted.begin(), dateSorted.end());

        const QString delimiter(" - ");
        QStringList intentions;

        foreach(const DateIntention& rec, dateSorted)
        {
            intentions << rec.m_date.toString(tr("ddd dd.MM.yyyy", "day in text day.month.year")) +
                delimiter + rec.m_intention;

        }

        SelectFromListDlg dlg(const_cast<ServiceDetailDlg*>(this), intentions);

        if (dlg.exec() == QDialog::Accepted)
        {
            return intentions.at(dlg.currentItemIndex()).section(delimiter, 1);
        }
    }
	return QString();
}

void ServiceDetailDlg::changeServiceDate()
{
    DateInputDlg dlg(this);
    const QDate originalDate = m_service->startTime().date();

    dlg.setDate(originalDate);

    if ((dlg.exec() == QDialog::Accepted) && (dlg.getDate() != originalDate))
    {
        const int serviceLength = m_service->lengthSeconds();
        const QDate newDate = dlg.getDate();

        // check for possible conflict
        HolyService tempService;
        QDateTime newStartDateTime = m_service->startTime();

        newStartDateTime.setDate(newDate);
        tempService.setStartTime(newStartDateTime);
        tempService.setEndTime(newStartDateTime.addSecs(serviceLength));
        tempService.setChurch(m_service->getChurch());
        tempService.setPriest(m_service->getPriest());

        HolyService* pConflicting;

        if (HolyServiceIndexing::allSet().conflicts(tempService, pConflicting))
        {
            const int answer = QMessageBox::question(this, tr("Replace?"),
                                                     tr("Holy service conflicts with service\n%1\n\n"
                                                        "Should the services be exchanged?\n(changes will be saved)").
                                                     arg((pConflicting) ? pConflicting->toString() : tr("the same service")),
                                                     QMessageBox::Yes | QMessageBox::No);

            if (pConflicting && (answer == QMessageBox::Yes))
            {
                dialogToObject();

                QDateTime temp;
                const int conflictingServiceLength = pConflicting->lengthSeconds();

                temp = pConflicting->startTime();
                temp.setDate(originalDate);
                pConflicting->setStartTime(temp);
                pConflicting->setEndTime(temp.addSecs(conflictingServiceLength));

                temp = m_service->startTime();
                temp.setDate(newDate);
                m_service->setStartTime(temp);
                m_service->setEndTime(temp.addSecs(serviceLength));

                // and set the new time
                // resort the allset if date is changed
                HolyServiceIndexing::allSet().sort();

                objectToDialog();
            }
        }
        else
        {
            m_service->setStartTime(newStartDateTime);
            m_service->setEndTime(newStartDateTime.addSecs(serviceLength));

            // resort the allset if date is changed
            HolyServiceIndexing::allSet().sort();

            m_ui->dateDisplay->setText(m_service->startTime().date().toString(Qt::LocaleDate));
        }
    }

}

void ServiceDetailDlg::churchIndexChanged(int index)
{
    QModelIndex modelIndex = m_ui->churchCombo->model()->index(index, 0);
    Church* pChurch = static_cast<Church *>(modelIndex.internalPointer());

    if (pChurch)
    {
        m_ui->churchDescriptionLabel->setText(pChurch->name());
    }
    else
    {
        m_ui->churchDescriptionLabel->setText(tr("<no church assigned>"));
    }

    // this call might seem superfluous, but it normalizes the internal combo box current index
    // as if the popup selection was always clicked in modelColumn() column so that
    // on leaving the edit field weird current index jumps do not happen
    if (index >= 0)
    {
        m_ui->churchCombo->setCurrentIndex(index);
    }
}

void ServiceDetailDlg::clientIndexChanged(int index)
{
    QModelIndex modelIndex = m_ui->clientCombo->model()->index(index, 0);
    Client* pClient = static_cast<Client *>(modelIndex.internalPointer());

    if (pClient)
    {
        m_ui->clientDescriptionLabel->setText(pClient->toString(Client::NameStreet));
    }
    else
    {
        m_ui->clientDescriptionLabel->setText(tr("<no client assigned>"));
    }

    // this call might seem superfluous, but it normalizes the internal combo box current index
    // as if the popup selection was always clicked in modelColumn() column so that
    // on leaving the edit field weird current index jumps do not happen
    if (index >= 0)
    {
        qDebug() << "Client combo index: " << index;
        m_ui->clientCombo->setCurrentIndex(index);
    }
}

void ServiceDetailDlg::clearClientAssignment()
{
    m_ui->clientCombo->setCurrentIndex(-1);
}

void ServiceDetailDlg::setNewClientNameHint(const QString &hint)
{
    m_newClientNameHint = hint;
}

void ServiceDetailDlg::addNewClient()
{
    ClientDetailDlg dlg(this);
    std::unique_ptr<Client> newClient(new Client);

    {
        const auto nameParts = m_newClientNameHint.split(QRegularExpression("\\s"), QString::SkipEmptyParts);
        if (nameParts.size() > 0)
        {
            newClient->setSurname(nameParts.at(0));
        }
        if (nameParts.size() > 1)
        {
            newClient->setFirstName(nameParts.mid(1).join(" "));
        }
    }

    dlg.setClient(newClient.get());
    if (dlg.exec() != QDialog::Accepted) return;

    newClient->setId(IdTag::createInitialized());
    ClientIndexing::allSet().addElement(newClient.get());

    m_ui->clientCombo->setModel(nullptr);
    m_ui->clientCombo->setModel(m_clientModel);

    selectServiceClient(newClient.get());

    newClient.release();
}

void ServiceDetailDlg::selectServiceClient(const Client* const serviceClient)
{
    const int clientIndex = (serviceClient) ?
        m_ui->clientCombo->findData(QVariant(serviceClient->getId().toString())) :
        -1;

    m_ui->clientCombo->setCurrentIndex(clientIndex);
    clientIndexChanged(clientIndex);
}

void ServiceDetailDlg::intentionGotFocus()
{
	static bool selectionAlreadyDisplayed = false;

//    qDebug() << "Intention got focus " << m_ui->intentionEdit->hasFocus();

	if ((! selectionAlreadyDisplayed) && 
        (m_ui->clientCombo->currentIndex() >= 0) &&
        m_ui->intentionEdit->toPlainText().trimmed().isEmpty())
	{
		selectionAlreadyDisplayed = true;

		const QString choice = choiceFromPreviousIntentions();

//		qDebug() << "Selected " << choice;
        m_ui->intentionEdit->setPlainText(choice);
        //m_ui->intentionEdit->setFocus();
		selectionAlreadyDisplayed = false;
	}
}

void ServiceDetailDlg::showRecordHistory()
{
    QString dateCreated = m_service->created().date().toString(Qt::DefaultLocaleShortDate);
    dateCreated += " " + m_service->created().time().toString("hh:mm");

    QString dateModified = m_service->lastModified().date().toString(Qt::DefaultLocaleShortDate);
    dateModified += " " + m_service->lastModified().time().toString("hh:mm");

    QString historyReport = tr("Created:\t%1 by %2\nModified:\t%3 by %4").
                            arg(dateCreated, m_service->creator(),
                                dateModified, m_service->lastModificator());

    QMessageBox::information(this, tr("Information"), historyReport);
}

void ServiceDetailDlg::selectHintedClient(const QModelIndex& idx)
{
    const auto hintedSurname = idx.data(Qt::DisplayRole);
    const auto clientModel = m_ui->clientCombo->model();
    const auto firstClient = clientModel->index(0, 0);
    const auto clientSearchResult = clientModel->match(firstClient, Qt::DisplayRole, hintedSurname, 1);
    if (! clientSearchResult.isEmpty())
    {
        m_ui->clientCombo->setCurrentIndex(clientSearchResult.first().row());
    }
}
