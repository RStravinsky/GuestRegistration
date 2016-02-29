#include "completerdelegate.h"

CompleterDelegate::CompleterDelegate()
{
    sqlModel = new ModSqlTableModel(this);
    sqlModel->setTable("registration");
    sqlModel->select();
}

void CompleterDelegate::setList(const int column, QStringList &stringList) const
{
    for(int i=0; i<sqlModel->rowCount();++i)
        if(! stringList.contains(sqlModel->index(i,column).data().toString()))
            stringList << sqlModel->index(i,column).data().toString();

}

