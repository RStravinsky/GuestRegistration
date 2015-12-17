#include "modsqltablemodel.h"

bool ModSqlTableModel::isGroup = false;

Qt::ItemFlags ModSqlTableModel::flags(const QModelIndex &index) const
{
    if(index.row() == rowCount()-1 && this->isDirty() ) {

        if(!isGroup) {
            if ((index.column() == 6 || index.column() == 7))
                return Qt::ItemIsEditable | Qt::ItemIsSelectable;
            else
                return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
        }
        else {
            if ((index.column() == 6 || index.column() == 7 || index.column() == 1 || index.column() == 2))
                return Qt::ItemIsEditable | Qt::ItemIsSelectable;
            else
                return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
        }
    }

    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ModSqlTableModel::data(const QModelIndex &idx, int role) const
{
    QVariant v = QSqlTableModel::data(idx, role);

    if ((role == Qt::BackgroundRole) && (index(idx.row(), 7, idx.parent()).data().toString().isEmpty()))
        return QVariant(QColor(5,88,86));

    return (v);
}
