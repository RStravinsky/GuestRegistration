#ifndef MODSQLTABLEMODEL_H
#define MODSQLTABLEMODEL_H

#include <QObject>
#include <QSqlTableModel>

class ModSqlTableModel : public QSqlTableModel
{

public:
    explicit ModSqlTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase()) : QSqlTableModel(parent, db) {}
    Qt::ItemFlags flags ( const QModelIndex & index ) const;


signals:

public slots:

private:


};

#endif // MODSQLTABLEMODEL_H
