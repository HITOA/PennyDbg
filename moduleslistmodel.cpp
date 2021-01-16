#include "moduleslistmodel.h"
#include <iostream>

ModulesListModel::ModulesListModel(QObject *parent) : QAbstractTableModel(parent)
{
}

int ModulesListModel::columnCount(const QModelIndex & /*parent*/) const {
    return ModuleListStruct_ElementSize;
}

int ModulesListModel::rowCount(const QModelIndex & /*parent*/) const {
    return listModules.size();
}

QVariant ModulesListModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return listModules.at(index.row()).moduleName;
                break;
            case 1:
                return listModules.at(index.row()).baseAdress;
                break;
            case 2:
                return listModules.at(index.row()).moduleSize;
                break;
            case 3:
                return listModules.at(index.row()).Description;
                break;
        }
    }
    return QVariant();
}

void ModulesListModel::AddItem(ModuleListStruct moduleListStruct) {
    listModules.push_back(moduleListStruct);
}
