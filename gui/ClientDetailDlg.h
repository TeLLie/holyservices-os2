#ifndef CLIENTDETAILDLG_H
#define CLIENTDETAILDLG_H

#include <QDialog>

namespace Ui {
class ClientDetailDlg;
}

class Client;

class ClientDetailDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ClientDetailDlg(QWidget *parent = nullptr);
    ~ClientDetailDlg();

    void setClient(Client *client);

private slots:
    void onOk();

private:
    Ui::ClientDetailDlg *m_ui = nullptr;
    Client* m_client = nullptr;

    void objectToDialog();
    void dialogToObject();
};

#endif // CLIENTDETAILDLG_H
