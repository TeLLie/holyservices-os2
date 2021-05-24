#include "CountMoneyContributionsDlg.h"
#include "ui_CountMoneyContributionsDlg.h"

#include "ButtonDecorator.h"
#include "HolyServiceIndexing.h"
#include "HsTypedefs.h"
#include "IServiceFilter.h"

#include <QClipboard>

QDate CountMoneyContributionsDlg::lastFrom;
QDate CountMoneyContributionsDlg::lastTo;

CountMoneyContributionsDlg::CountMoneyContributionsDlg(QWidget *parent) :
    super(parent),
    m_ui(new Ui::CountMoneyContributionsDlg)
{
    m_ui->setupUi(this);
    m_filter = nullptr;

    connect(m_ui->computeButton, &QAbstractButton::clicked,
            this, &CountMoneyContributionsDlg::computeMoneySum);

    if (lastFrom.isValid() && lastTo.isValid())
    {
        m_ui->fromEdit->setDate(lastFrom);
        m_ui->toEdit->setDate(lastTo);
    }
    else
    {
        const QDate currentDate = QDate::currentDate();
        m_ui->fromEdit->setDate(QDate(currentDate.year(), 1, 1));
        m_ui->toEdit->setDate(currentDate);
    }

    m_ui->computationReportLabel->hide();

    ButtonDecorator::assignIcons(*m_ui->buttonBox);
}

CountMoneyContributionsDlg::~CountMoneyContributionsDlg()
{
    delete m_ui;
}

void CountMoneyContributionsDlg::setFilter(IServiceFilter *f)
{
    m_filter = f;
}

void CountMoneyContributionsDlg::computeMoneySum()
{
    const QDate from = m_ui->fromEdit->date();
    const QDate to = m_ui->toEdit->date();

    const tHolyServiceVector inRange = HolyServiceIndexing::allSet().query(from, to, m_filter);
    double moneySum = 0.0;

    foreach(const HolyService* hs, inRange)
    {
        moneySum += hs->receivedMoney();
    }

    const QString moneySumText = QString("%L1").arg(moneySum);

    m_ui->computationReportLabel->setText(
                tr("Computed over %L1 holy services, result transferred into clipboard").arg(inRange.size()));
    m_ui->computationReportLabel->show();
    m_ui->resultEdit->setText(moneySumText);

    // put in clipboard
    QApplication::clipboard()->setText(moneySumText);

    lastFrom = from; lastTo = to;
}
