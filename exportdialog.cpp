#include "exportdialog.h"
#include "ui_exportdialog.h"

ExportDialog::ExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);

    sqlModel = new ModSqlTableModel;
    sqlModel->setTable("registration");
    sqlModel->sort(6,Qt::AscendingOrder);
    sqlModel->select();

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

    ui->dataComboBox->setVisible(false);
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
    if(ui->pathLineEdit->text().isEmpty()) {
        QMessageBox::information(this,QString("Informacja"),QString("Nie wybrano pliku do eksportu."));
        return;
    }

    if(ui->categoriesComboBox->currentText() == "Data") {
        QDate startDate = QDate::fromString(ui->startDateLineEdit->text(),"yyyy-MM-dd");
        QDate stopDate = QDate::fromString(ui->stopDateLineEdit->text(),"yyyy-MM-dd");

        if(ui->startDateLineEdit->text().isEmpty() || ui->stopDateLineEdit->text().isEmpty()) {
            QMessageBox::information(this,QString("Informacja"),QString("Daty muszą być uzupełnione."));
            return;
        }
        else if(startDate > stopDate ) {
            QMessageBox::information(this,QString("Informacja"),QString("Data końcowa jest większa niż data początkowa."));
            return;
        }
        sqlModel->setFilter("ArrivalTime between '"+startDate.toString("yyyy-MM-dd")+"' And '"+stopDate.toString("yyyy-MM-dd")+"'");
    }
    else {
        QString headerName;
        if(ui->categoriesComboBox->currentText() == "Nazwisko") headerName = QString("Surname");
        else if(ui->categoriesComboBox->currentText() == "Firma") headerName = QString("Company");
        else if(ui->categoriesComboBox->currentText() == "Tablica rejestracyjna") headerName = QString("LicensePlate");
        sqlModel->setFilter(""+headerName+" LIKE '%"+ui->dataComboBox->currentText()+"%'");
    }

    populatePDF();
    ExportDialog::accept();
}

void ExportDialog::on_pathButton_clicked()
{
    QString pdfFile;
    QString filters( "Pliki PDF (*.pdf);;Wszystkie pliki (*.*)" );
    QString pdfFilter( "Pliki PDF (*.pdf)" );

    pdfFile = QFileDialog::getSaveFileName( this, "Wyeksportuj do pliku", QDir::homePath(), filters, &pdfFilter );
    ui->pathLineEdit->setText(pdfFile);
    ui->pathLineEdit->setCursorPosition(0);
}

QTextTableFormat ExportDialog::tableFormat()
{
    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignCenter);
    tableFormat.setCellSpacing(0);
    tableFormat.setCellPadding(3);
    tableFormat.setTopMargin(10);
    tableFormat.setBottomMargin(10);
    tableFormat.setBorderBrush(Qt::black);

    QVector<QTextLength> widths;
    widths << QTextLength(QTextLength::PercentageLength, 2) << QTextLength(QTextLength::PercentageLength, 14) << QTextLength(QTextLength::PercentageLength, 15)
           << QTextLength(QTextLength::PercentageLength, 9) << QTextLength(QTextLength::PercentageLength, 8) << QTextLength(QTextLength::PercentageLength, 10)
           << QTextLength(QTextLength::PercentageLength, 18) << QTextLength(QTextLength::PercentageLength, 24);

    tableFormat.setColumnWidthConstraints(widths);
    return tableFormat;
}

void ExportDialog::addHeaderToDocument(QTextCursor *cursor, QString &title)
{
    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(Qt::AlignLeft);
    blockFormat.setTopMargin(0);
    //cursor->insertBlock(blockFormat);
    //cursor->insertImage(":/images/images/sigma.png");
    //blockFormat.setLeftMargin(40);
    QTextCharFormat charFormat;
    charFormat.setFont(QFont("Calibri", 9, QFont::Bold));
    cursor->insertText(title, charFormat);
}

void ExportDialog::populateTable(QTextCursor *cursor)
{
    QTextCursor tableCursor;
    QTextBlockFormat blockFormat;
    QTextTable *table = cursor->insertTable(sqlModel->rowCount() + 1, sqlModel->columnCount(), tableFormat());

    QStringList headerList ({"L.p.","Imię", "Nazwisko", "Firma", "Tablica rejestracyjna", "Cel wizyty", "Czas przyjazdu", "Czas wyjazdu"});

    for(int i = 1; i < sqlModel->rowCount() + 1; ++i) {
        tableCursor = table->cellAt(i,0).firstCursorPosition();
        blockFormat.setAlignment(Qt::AlignHCenter);
        tableCursor.insertBlock(blockFormat);
        tableCursor.insertText(QString::number(i));
    }

     for(int j = 0; j < sqlModel->columnCount(); ++j) {
         tableCursor = table->cellAt(0,j).firstCursorPosition();
         blockFormat.setAlignment(Qt::AlignHCenter);
         tableCursor.insertBlock(blockFormat);
         tableCursor.insertText(headerList.at(j));
     }

    for(int i = 1; i < sqlModel->rowCount() + 1; ++i) {

        for(int j = 1; j < sqlModel->columnCount(); ++j) {

            tableCursor = table->cellAt(i,j).firstCursorPosition();

            if(j == 6 || j == 7) {
                blockFormat.setAlignment(Qt::AlignHCenter);
                tableCursor.insertBlock(blockFormat);
                tableCursor.insertText(sqlModel->index(i-1, j).data().toString().replace("T"," "));
            }
            else {
                blockFormat.setAlignment(Qt::AlignHCenter);
                tableCursor.insertBlock(blockFormat);
                tableCursor.insertText(sqlModel->index(i-1, j).data().toString());
            }
        }
    }
}

void ExportDialog::populatePDF()
{
    QTextDocument document;
    document.setDefaultFont(QFont("Calibri", 9));
    QTextCursor textCursor(&document);
    QString headerText = "Wygenerowano: " + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss");

    addHeaderToDocument(&textCursor, headerText);
    populateTable(&textCursor);

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFileName(ui->pathLineEdit->text());
    printer.setOutputFormat(QPrinter::PdfFormat);
    document.print(&printer);

    QMessageBox::information( this, "Informacja", "Wyeksportowano do pliku:\n" + ui->pathLineEdit->text() );

    QDesktopServices::openUrl(ui->pathLineEdit->text());
}


void ExportDialog::on_categoriesComboBox_currentIndexChanged(const QString &arg1)
{
    static QString previousArg1{};

    if (previousArg1 != arg1) {

        QString headerName;
        [arg1,&headerName] {
            if(arg1 == "Nazwisko") headerName = QString("Surname");
            else if(arg1 == "Firma") headerName = QString("Company");
            else if(arg1 == "Tablica rejestracyjna") headerName = QString("LicensePlate");
            else headerName = QString("Date");
        }();

        if(headerName == "Date") {
            ui->startDateButton->setVisible(true);
            ui->startDateLineEdit->setVisible(true);
            ui->stopDateButton->setVisible(true);
            ui->stopDateLineEdit->setVisible(true);
            ui->dataComboBox->setVisible(false);
        }
        else {
            ui->startDateButton->setVisible(false);
            ui->startDateLineEdit->setVisible(false);
            ui->stopDateButton->setVisible(false);
            ui->stopDateLineEdit->setVisible(false);
            ui->dataComboBox->setVisible(true);
        }

        int categoriesIdx = 0;
        for(int i = 0; i < sqlModel->columnCount(); ++i)
            if(sqlModel->headerData(i, Qt::Horizontal).toString() == headerName)
                categoriesIdx = i;

       ui->dataComboBox->clear();
       for(int i=0; i<sqlModel->rowCount();++i) {
            QString data = sqlModel->index(i,categoriesIdx).data().toString();
            if(!data.isEmpty() && data!=QString("-----"))
            ui->dataComboBox->addItem(data);
       }
   }

   previousArg1 = arg1;
}
