#include "exportdialog.h"
#include "ui_exportdialog.h"

ExportDialog::ExportDialog(ModSqlTableModel * model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);

    sqlModel = new ModSqlTableModel(model);

    // start Calendar
    QMenu* startMenu = new QMenu(ui->startDateButton);
    startCalendar = new QCalendarWidget(startMenu);
    startCalendar -> setMinimumDate( QDate(2015,12,01) );
    startCalendar -> setMaximumDate( QDate(7999,01,01) );
    naviBarColor(startCalendar);
    QWidgetAction* startAction = new QWidgetAction(startMenu);
    startAction->setDefaultWidget(startCalendar);
    startMenu->addAction(startAction);
    ui->startDateButton->setMenu(startMenu);
    connect( startCalendar, SIGNAL(clicked(QDate)), this, SLOT(on_dateClicked(QDate)) );

    // stop Calendar
    QMenu* stopMenu = new QMenu(ui->stopDateButton);
    stopCalendar = new QCalendarWidget(startMenu);
    stopCalendar -> setMinimumDate( QDate(2015,12,01) );
    stopCalendar -> setMaximumDate( QDate(7999,01,01) );
    naviBarColor(stopCalendar);
    QWidgetAction *stopAction = new QWidgetAction(stopMenu);
    stopAction->setDefaultWidget(stopCalendar);
    stopMenu->addAction(stopAction);
    ui->stopDateButton->setMenu(stopMenu);
    connect( stopCalendar, SIGNAL(clicked(QDate)), this, SLOT(on_dateClicked(QDate)) );

    connect(ui->cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::naviBarColor(QCalendarWidget *&calendarWidget)
{
    QWidget *calendarNavBar = calendarWidget->findChild<QWidget *>("qt_calendar_navigationbar");
    if (calendarNavBar) {
        QPalette pal = calendarNavBar->palette();
        pal.setColor(calendarNavBar->backgroundRole(), QColor(8, 142, 140));
        calendarNavBar->setPalette(pal);
    }
}

void ExportDialog::on_dateClicked(QDate date)
{
    if(date > QDate::currentDate())
        QMessageBox::information(this,QString("Informacja"),QString("Wybrana data nie może przekraczać dzisiejszej daty."));
    else {
        if( qobject_cast<QCalendarWidget *>(sender()) == startCalendar)
            ui->startDateLineEdit->setText(date.toString("yyyy-MM-dd"));
        else if(qobject_cast<QCalendarWidget *>(sender()) == stopCalendar )
            ui->stopDateLineEdit->setText(date.toString("yyyy-MM-dd"));
    }
}


void ExportDialog::on_exportButton_clicked()
{
    QDate startDate = QDate::fromString(ui->startDateLineEdit->text(),"yyyy-MM-dd");
    QDate stopDate = QDate::fromString(ui->stopDateLineEdit->text(),"yyyy-MM-dd");
    if(startDate > stopDate )
                QMessageBox::information(this,QString("Informacja"),QString("Data końcowa jest większa niż data początkowa."));
    else {
        sqlModel->setFilter("ArrivalTime between "+startDate.toString("yyyy-MM-dd")+" And "+stopDate.toString("yyyy-MM-dd")+"");
        // exportToPDF
        ExportDialog::accept();
    }

}
