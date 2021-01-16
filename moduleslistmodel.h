#ifndef MODULESLISTMODEL_H
#define MODULESLISTMODEL_H

#include <QAbstractItemModel>

#define ModuleListStruct_ElementSize 4

typedef struct _ModuleListStruct {
    QString moduleName;
    QString baseAdress;
    QString moduleSize;
    QString Description;
}ModuleListStruct, *PModuleListStruct;

class ModulesListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    std::vector<ModuleListStruct> listModules;
    ModulesListModel(QObject *parent);
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void AddItem(ModuleListStruct moduleListStruct);
};

#endif // MODULESLISTMODEL_H
