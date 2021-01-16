#ifndef MODULESTABLE_H
#define MODULESTABLE_H

#define MODULESTABLE_COLUMN_COUNT 3

#include <QAbstractTableModel>

class ModulesTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ModulesTableModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // MODULESTABLE_H
