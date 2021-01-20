#ifndef THREADTABLEMODEL_H
#define THREADTABLEMODEL_H

#include <QString>
#include <QAbstractTableModel>
#include <debuggedprocessdata.h>

#define THREADSTABLEMODEL_COLUMN 3;

class ThreadTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    LPDebuggedProcessData lpPData;
    ThreadTableModel(LPDebuggedProcessData lpPData, QObject *parent);
    void updateGUI();
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QString parseThreadPriority(int threadPriority) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // THREADTABLEMODEL_H
