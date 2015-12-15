#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QMenu>
#include <QCalendarWidget>
#include <QWidgetAction>
#include <QMessageBox>
#include <QTextCursor>
#include <QFileDialog>
#include <QTextTable>
#include <QPrinter>
#include <QtPrintSupport>
#include <QDesktopServices>
#include <modsqltablemodel.h>

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(ModSqlTableModel *model,QWidget *parent = 0);
    ~ExportDialog();

private slots:
    void on_dateClicked(QDate date);
    void on_exportButton_clicked();
    void on_pathButton_clicked();

private:
    Ui::ExportDialog *ui;
    QCalendarWidget* startCalendar;
    QCalendarWidget* stopCalendar;
    ModSqlTableModel * sqlModel;
    void naviBarColor(QCalendarWidget *& calendarWidget);

    void populatePDF();
    QTextTableFormat tableFormat();
    void addHeaderToDocument(QTextCursor *cursor, QString &title);
    void populateTable(QTextCursor *cursor);
};

#endif // EXPORTDIALOG_H
