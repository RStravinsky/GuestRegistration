#ifndef COMPLETERDELEGATE_H
#define COMPLETERDELEGATE_H

#include <QStyledItemDelegate>
#include <QCompleter>
#include <QLineEdit>
#include <QAbstractItemView>
#include <QScrollBar>
#include <modsqltablemodel.h>

class CompleterDelegate : public QStyledItemDelegate
{
public:
    CompleterDelegate();
    ModSqlTableModel * sqlModel;
    void setList(const int column,QStringList & stringList) const;
    QStringList mutable stringList;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        Q_UNUSED(option)
        QLineEdit* lineEdit = new QLineEdit(parent);
        stringList.clear();
        setList(index.column(),stringList);
        QCompleter* completer = new QCompleter(stringList, lineEdit);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->popup()->verticalScrollBar()->setStyleSheet(
                                          "QScrollBar:vertical {background: rgb(35,35,35);width: 15px;margin: 20px 0px 20px 0px;}"
                                          "QScrollBar::handle:vertical{ background: rgb(100,100,100); width: 20px;}"
                                          "QScrollBar::add-line:vertical{ background: rgb(100,100,100); height: 15px; subcontrol-position: bottom;subcontrol-origin: margin;}"
                                          "QScrollBar::sub-line:vertical { background: rgb(100,100,100);height: 15px;subcontrol-position: top;subcontrol-origin: margin;}"
                                          "QScrollBar:down-arrow:vertical {width: 10px;height: 10px;background: rgb(100,100,100);image: url(:/images/images/downArrow.png);}"
                                          "QScrollBar:up-arrow:vertical {width: 10px;height: 10px;background: rgb(100,100,100);image: url(:/images/images/upArrow.png);}"
                                          "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: none;}");
        completer->popup()->setStyleSheet("QListView{background: rgb(35,35,35);color:white; font-size: 12pt; font-family: Calibri Light; font-weight:normal}");
        lineEdit->setCompleter(completer);
        return lineEdit;
    }
};

#endif // COMPLETERDELEGATE_H
