#include "threadtablemodel.h"

ThreadTableModel::ThreadTableModel(LPDebuggedProcessData lpPData, QObject *parent) : QAbstractTableModel(parent)
{
    this->lpPData = lpPData;
}

void ThreadTableModel::updateGUI() {
    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);
    emit dataChanged(topLeft, bottomRight);
    emit layoutChanged();
}

int ThreadTableModel::columnCount(const QModelIndex & /*parent*/) const {
    return THREADSTABLEMODEL_COLUMN;
}

int ThreadTableModel::rowCount(const QModelIndex & /*parent*/) const {
    return lpPData->GetThreadsSize();
}

QString ThreadTableModel::parseThreadPriority(int threadPriority) const {
    switch (threadPriority) {
        case 1:
            return QString("Above normal");
            break;
        case -1:
            return QString("Below normal");
            break;
        case 2:
            return QString("Highest");
            break;
        case -15:
            return QString("IDLE");
            break;
        case -2:
            return QString("Lowest");
            break;
        case 0:
            return QString("Normal");
            break;
        case 15:
            return QString("Time critical");
            break;
        default:
            return QString("Unknown");
            break;
    }
}

QVariant ThreadTableModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return QString::number(lpPData->GetThreadData(index.row())->dwThreadId);
                break;
            case 1:
                return QString("0x%1").arg((quintptr)lpPData->GetThreadData(index.row())->lpStartAdress, QT_POINTER_SIZE * 2, 16,QChar('0'));
                break;
            case 2:
                return parseThreadPriority(lpPData->GetThreadData(index.row())->priority);
                break;
            default:
                break;
        }
    }
    return QVariant();
}

