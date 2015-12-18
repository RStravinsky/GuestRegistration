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

    if (role == Qt::BackgroundRole) {

        if(isDirty() && !isGroup) {

            QString name = index(idx.row(), 1, idx.parent()).data().toString();
            QString surname = index(idx.row(), 2, idx.parent()).data().toString();
            if((name.isEmpty() || name.contains(" ")) && idx.column() == 1 && idx.row() == rowCount()-1)
                    return QVariant(QColor(10,158,155));
            if((surname.isEmpty() || surname.contains(" ")) && idx.column() == 2 && idx.row() == rowCount()-1)
                    return QVariant(QColor(10,158,155));
            else if ((role == Qt::BackgroundRole) && index(idx.row(), 7, idx.parent()).data().toString().isEmpty())
                    return QVariant(QColor(5,88,86));

        }
        else if (isDirty() && isGroup) {
            QString company = index(idx.row(), 3, idx.parent()).data().toString();
            if(company.isEmpty() && idx.column() == 3 && idx.row() == rowCount()-1)
                return QVariant(QColor(10,158,155));
            else if (index(idx.row(), 7, idx.parent()).data().toString().isEmpty()) {
                return QVariant(QColor(5,88,86));
            }

        }

        else if(!isDirty() && index(idx.row(), 7, idx.parent()).data().toString().isEmpty())
                  return QVariant(QColor(5,88,86));
    }

    return (v);
}


