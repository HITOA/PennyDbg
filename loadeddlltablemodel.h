#ifndef LOADEDDLLTABLEMODEL_H
#define LOADEDDLLTABLEMODEL_H

#define LOADEDDLLTABLEMODEL_COLUMN 4

#include <QAbstractItemModel>
#include "debuggedprocessdata.h"

class LoadedDllTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    LPDebuggedProcessData lpPData;
    LoadedDllTableModel(LPDebuggedProcessData lpPData, QObject *parent);
    void updateGUI();
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // LOADEDDLLTABLEMODEL_H
