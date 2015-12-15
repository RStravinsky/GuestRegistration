#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QMenu>
#include <QCalendarWidget>
#include <QWidgetAction>
#include <QMessageBox>
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

private:
    Ui::ExportDialog *ui;
    QCalendarWidget* startCalendar;
    QCalendarWidget* stopCalendar;
    ModSqlTableModel * sqlModel;

    void naviBarColor(QCalendarWidget *& calendarWidget);
};

#endif // EXPORTDIALOG_H
