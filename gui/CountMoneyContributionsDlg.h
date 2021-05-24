#ifndef COUNTMONEYCONTRIBUTIONSDLG_H
#define COUNTMONEYCONTRIBUTIONSDLG_H

#include <QDialog>
#include <QDate>

namespace Ui
{
    class CountMoneyContributionsDlg;
}
class IServiceFilter;

class CountMoneyContributionsDlg : public QDialog
{
    Q_OBJECT
    typedef QDialog super;

public:
    explicit CountMoneyContributionsDlg(QWidget *parent = nullptr);
    ~CountMoneyContributionsDlg();

    void setFilter(IServiceFilter*);

private slots:
    void computeMoneySum();

private:
    static QDate lastFrom;
    static QDate lastTo;

    Ui::CountMoneyContributionsDlg *m_ui;
    IServiceFilter* m_filter;
};

#endif // COUNTMONEYCONTRIBUTIONSDLG_H
