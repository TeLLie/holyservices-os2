/*!
  \class LocalRemoteDiffModel
  \author Jan 'kovis' Struhar
*/
#ifndef LOCALREMOTEDIFFMODEL_H
#define LOCALREMOTEDIFFMODEL_H

#include "localandremote.h"

#include <QAbstractTableModel>
#include <QVector>

class LocalRemoteDiffModel : public QAbstractTableModel
{
    Q_OBJECT
    typedef QAbstractTableModel super;
public:
    explicit LocalRemoteDiffModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setDiffData(const QVector<LocalAndRemote> &diffData);

private:
    enum Column { LOCAL_DATA = 0, REMOTE_DATA, COLUMN_COUNT };
    QVector<LocalAndRemote> m_diffData;
};

#endif // LOCALREMOTEDIFFMODEL_H
