#ifndef ADDSINGLEHOLIDAYDLG_H
#define ADDSINGLEHOLIDAYDLG_H

///\class AddSingleHolidayDlg
/// Gather data for simple holiday definition (description and next years)
///\date 10-2011
///\author Jan 'Kovis' Struhar

#include <QDialog>
#include <QDate>

namespace Ui
{
    class AddSingleHolidayDlg;
}

class AddSingleHolidayDlg : public QDialog
{
    Q_OBJECT
    typedef QDialog super;

public:
    explicit AddSingleHolidayDlg(QWidget *parent = nullptr);
    ~AddSingleHolidayDlg();


    void setDate(const QDate& date);

    QString description() const; ///< holiday description
    bool forNextYears() const; ///< define holiday also in following years

private:
    Ui::AddSingleHolidayDlg *m_ui;
    QDate m_date;
};

#endif // ADDSINGLEHOLIDAYDLG_H
