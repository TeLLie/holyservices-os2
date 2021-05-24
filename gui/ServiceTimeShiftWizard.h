#ifndef SERVICETIMESHIFTWIZARD_H
#define SERVICETIMESHIFTWIZARD_H

/*!
  \class ServiceTimeShiftWizard
  Wizard that controls moving of set of services in time
  \author Jan 'Kovis' Struhar
*/

#include "HsTypedefs.h"
#include <QWizard>
#include <QAbstractTableModel>
#include <chrono>

namespace Ui
{
    class ServiceTimeShiftWizard;
}

class WeekModel : public QAbstractTableModel
{
    Q_OBJECT
    typedef QAbstractTableModel super;

public:
    WeekModel(QObject* p);
    void setData(const tHolyServiceVector &sourceData);

    enum Column { Monday = 0, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday, ColumnCount };

    // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    tHolyServiceVector m_data;
    int m_weekStart = -1;
    int m_weekEnd = -1;

    tConstHolyServiceVector servicesOnIndex(const QModelIndex& index) const;
};

class ShiftedServicesTableModel : public QAbstractTableModel
{
    Q_OBJECT
    typedef QAbstractTableModel super;
public:
    ShiftedServicesTableModel(QObject* p);

    void setData(const tHolyServiceVector &sourceData);
    void setTimeShift(const std::chrono::minutes &timeShift);
    tHolyServiceVector toBeShiftedData() const;

    enum Column { Intention = 0, OriginalTime, ShiftedTime, ColumnCount };

    // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    tHolyServiceVector m_data;
    std::chrono::minutes m_timeShift = std::chrono::minutes(0);
};

class ServiceTimeShiftWizard : public QWizard
{
    Q_OBJECT

public:
    explicit ServiceTimeShiftWizard(QWidget *parent = nullptr);
    ~ServiceTimeShiftWizard();

    void setData(const tHolyServiceVector &sourceData);
    tHolyServiceVector servicesToBeShifted() const;
    std::chrono::minutes wantedTimeShift() const;

private slots:
    void on_yesterdayButton_clicked();
    void on_oneHourEarlier_clicked();
    void on_oneHourLater_clicked();
    void on_tomorrowBtn_clicked();

    void on_ServiceTimeShiftWizard_currentIdChanged(int id);
    void on_timeShiftMinutesBox_valueChanged(int currentTimeShift);

private:
    enum Page { DATES_PAGE = 0, SELECTION_PAGE, SHIFT_PAGE };
    Ui::ServiceTimeShiftWizard *m_ui = nullptr;
    tHolyServiceVector m_data;
    tHolyServiceVector m_dataForSelection;
    WeekModel* m_weekModel = nullptr;
    ShiftedServicesTableModel* m_shiftedModel = nullptr;
    int m_previousId = 0;
};

#endif // SERVICETIMESHIFTWIZARD_H
