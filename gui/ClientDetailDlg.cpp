/***************************************************************************
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
***************************************************************************/

#include "ClientDetailDlg.h"
#include "ui_ClientDetailDlg.h"

#include "ButtonDecorator.h"
#include "Client.h"

ClientDetailDlg::ClientDetailDlg(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ClientDetailDlg)
{
    m_ui->setupUi(this);
    ButtonDecorator::assignIcons(*m_ui->buttonBox);
    connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &ClientDetailDlg::onOk);
}

ClientDetailDlg::~ClientDetailDlg()
{
    delete m_ui;
}

void ClientDetailDlg::setClient(Client *client)
{
    m_client = client;
    objectToDialog();
}

void ClientDetailDlg::onOk()
{
    if (m_ui->surnameEdit->text().trimmed().isEmpty())
    {
        return;
    }
    dialogToObject();
    accept();
}

void ClientDetailDlg::objectToDialog()
{
    if (m_client == nullptr) return;

    m_ui->nameEdit->setText(m_client->firstName());
    m_ui->surnameEdit->setText(m_client->surname());
    m_ui->addressEdit->setText(m_client->street());
    m_ui->contactEdit->setText(m_client->contact());
}

void ClientDetailDlg::dialogToObject()
{
    m_client->setFirstName(m_ui->nameEdit->text().trimmed());
    m_client->setSurname(m_ui->surnameEdit->text().trimmed());
    m_client->setStreet(m_ui->addressEdit->text().trimmed());
    m_client->setContact(m_ui->contactEdit->text().trimmed());
}
