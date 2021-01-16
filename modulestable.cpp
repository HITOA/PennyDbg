#include "modulestable.h"

ModulesTableModel::ModulesTableModel(QObject *parent) : QAbstractTableModel(parent)
{
}

int ModulesTableModel::rowCount(const QModelIndex & /*parent*/) const
{
   return 2;
}

int ModulesTableModel::columnCount(const QModelIndex & /*parent*/) const
{
    return MODULESTABLE_COLUMN_COUNT;
}

QVariant ModulesTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
       return QString("Row%1, Column%2")
                   .arg(index.row() + 1)
                   .arg(index.column() +1);

    return QVariant();
}
