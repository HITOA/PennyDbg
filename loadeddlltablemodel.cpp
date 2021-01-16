#include "loadeddlltablemodel.h"

LoadedDllTableModel::LoadedDllTableModel(LPDebuggedProcessData lpPData, QObject *parent) : QAbstractTableModel(parent)
{
    this->lpPData = lpPData;
}

void LoadedDllTableModel::updateGUI() {
    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);
    emit dataChanged(topLeft, bottomRight);
    emit layoutChanged();
}

int LoadedDllTableModel::columnCount(const QModelIndex & /*parent*/) const {
    return LOADEDDLLTABLEMODEL_COLUMN;
}

int LoadedDllTableModel::rowCount(const QModelIndex & /*parent*/) const {
    return lpPData->GetLoadedDllsSize();
}

QVariant LoadedDllTableModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return QString::fromWCharArray(lpPData->GetLoadedDllData(index.row())->fullName);
                break;
            case 1:
                return QString("0x%1").arg((quintptr)lpPData->GetLoadedDllData(index.row())->lpBaseOfDll, QT_POINTER_SIZE * 2, 16,QChar('0'));
                break;
            case 2:
                return QString::number(lpPData->GetLoadedDllData(index.row())->dllMappedSize);
                break;
            case 3:
                return QString::number(lpPData->GetLoadedDllData(index.row())->dllDiskSize);
                break;
            default:
                break;
        }
    }
    return QVariant();
}
