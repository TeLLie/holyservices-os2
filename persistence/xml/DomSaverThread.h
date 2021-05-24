#ifndef DOMSAVERTHREAD_H_INCLUDED_1ACE839B_5995_4C84_B7C0_28A77F011822
#define DOMSAVERTHREAD_H_INCLUDED_1ACE839B_5995_4C84_B7C0_28A77F011822

///\class DomSaverThread
/// The thread transforms XML DOM document to text and saves it
///\date 1-2007
///\author Jan 'Kovis' Struhar

#include <QThread>
#include <QString>
#include <QDomDocument>


class DomSaverThread : public QThread
{
	Q_OBJECT
	typedef QThread super;
public:
    DomSaverThread();
    virtual ~DomSaverThread();

    void run(void) override;
	
	void setData(const QDomDocument& savedDocument, const QString& fileName);

signals:
	void errorMessage(QString);

private:
	QString m_fileName;
	QDomDocument m_toBeSaved;
};

#endif
