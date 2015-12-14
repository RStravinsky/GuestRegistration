#include "modsqltablemodel.h"


Qt::ItemFlags ModSqlTableModel::flags(const QModelIndex &index) const
{

    if(index.row() == rowCount()-1 && this->isDirty() ) {

    if ((index.column() == 6 || index.column() == 7))
        return Qt::ItemIsEditable | Qt::ItemIsSelectable;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

    }

    else {

        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

}
