#ifndef SERVICEDETAILDLG_H_INCLUDED_D259F8D7_F8A9_4493_9D2A_5D2CAACFB956
#define SERVICEDETAILDLG_H_INCLUDED_D259F8D7_F8A9_4493_9D2A_5D2CAACFB956

class HolyService;
class Client;
class QFocusEvent;
class QAbstractItemModel;
class QAbstractItemView;
class Ui_ServiceDetailDlg;

#include <QDialog>

class ServiceDetailDlg : public QDialog
{
	typedef QDialog super;
	Q_OBJECT
public:
    ServiceDetailDlg(QWidget *parent);
    virtual ~ServiceDetailDlg();

    void setService(HolyService *);
    void setNewClientNameHint(const QString& hint);

private slots:
	void okPressed();
	void changeServiceDate();
	void churchIndexChanged(int);
	void clientIndexChanged(int);
	void clearClientAssignment();
    void addNewClient();
    void intentionGotFocus();
    void showRecordHistory();
    void selectHintedClient(const QModelIndex& idx);

private:
    Ui_ServiceDetailDlg* m_ui = nullptr;
    HolyService *m_service = nullptr;
    QAbstractItemModel *m_churchModel = nullptr;
    QAbstractItemModel *m_priestModel = nullptr;
    QAbstractItemModel *m_clientModel = nullptr;
    QString m_newClientNameHint;

	void objectToDialog();
	void dialogToObject();

	/// reports a message box if the service is suspicious
	bool serviceValidityCheck() const;

    QString choiceFromPreviousIntentions() const;
    void selectServiceClient(const Client* const serviceClient);
};

#endif
