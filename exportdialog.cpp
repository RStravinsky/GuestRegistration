#include "exportdialog.h"
#include "ui_exportdialog.h"

ExportDialog::ExportDialog(ModSqlTableModel * model, QWidget *parent) :
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
        sqlModel->setFilter("ArrivalTime between '"+startDate.toString("yyyy-MM-dd")+"' And '2015-12-16'");
        populatePDF();
        ExportDialog::accept();
    }
}

void ExportDialog::on_pathButton_clicked()
{
    QString pdfFile;
    QString filters( "Pliki PDF (*.pdf);;Wszystkie pliki (*.*)" );
    QString pdfFilter( "Pliki PDF (*.pdf)" );

    pdfFile = QFileDialog::getSaveFileName( this, "Wyeksportuj do pliku", QDir::homePath(), filters, &pdfFilter );
    ui->pathLineEdit->setText(pdfFile);
}

QTextTableFormat ExportDialog::tableFormat(TabFormat format)
{
    if(format == TabFormat::Table) {

        QTextTableFormat tableFormat;
        tableFormat.setAlignment(Qt::AlignCenter);
        tableFormat.setCellSpacing(0);
        tableFormat.setCellPadding(3);
        tableFormat.setTopMargin(10);
        tableFormat.setBottomMargin(10);
        tableFormat.setBorderBrush(Qt::black);

        QVector<QTextLength> widths;
        widths << QTextLength(QTextLength::PercentageLength, 4) << QTextLength(QTextLength::PercentageLength, 14) << QTextLength(QTextLength::PercentageLength, 15)
               << QTextLength(QTextLength::PercentageLength, 7) << QTextLength(QTextLength::PercentageLength, 8) << QTextLength(QTextLength::PercentageLength, 10)
               << QTextLength(QTextLength::PercentageLength, 18) << QTextLength(QTextLength::PercentageLength, 24);

        tableFormat.setColumnWidthConstraints(widths);
        return tableFormat;

    }

    if(format == TabFormat::Header) {

        QTextTableFormat tableFormat;
        tableFormat.setAlignment(Qt::AlignCenter);
        tableFormat.setCellSpacing(0);
        tableFormat.setCellPadding(3);
        tableFormat.setTopMargin(10);
        tableFormat.setBottomMargin(0);
        tableFormat.setBorder(0);


        QVector<QTextLength> widths;
        widths << QTextLength(QTextLength::PercentageLength, 10) << QTextLength(QTextLength::PercentageLength, 45) << QTextLength(QTextLength::PercentageLength, 45);

        tableFormat.setColumnWidthConstraints(widths);
        return tableFormat;
    }


}

void ExportDialog::addHeaderToDocument(QTextDocument *document,QTextCursor *cursor)
{
    QImage logoImage(":/images/images/sigma.png");
    document->addResource(QTextDocument::ImageResource, QUrl(":/images/images/sigma.png"), logoImage);

    QTextTable *headerTable = cursor->insertTable(1, 3, tableFormat(TabFormat::Header));
    //headerTable->mergeCells(1,0,1,3);

    QTextBlockFormat blockFormat;
    QTextImageFormat logoFormat;
    logoFormat.setName(":/images/images/sigma.png");
    logoFormat.setWidth(40);

    headerTable->cellAt(0,0).firstCursorPosition();

    blockFormat.setAlignment(Qt::AlignBottom);
    cursor->insertBlock(blockFormat);
    cursor->insertImage(logoFormat);


    cursor->movePosition(QTextCursor::NextCell);
    blockFormat.setAlignment(Qt::AlignRight);
    cursor->insertBlock(blockFormat);
    QTextCharFormat charFormat;
    charFormat.setFont(QFont("Calibri", 12, QFont::Bold));
    cursor->insertText("RAPORT EWIDENCJI GOŚCI", charFormat);

    QString headerText = "Wygenerowano: " + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss");

    cursor->movePosition(QTextCursor::NextCell);
    blockFormat.setAlignment(Qt::AlignRight);
    cursor->insertBlock(blockFormat);
    QTextCharFormat charFormat2;
    charFormat2.setFont(QFont("Calibri", 9));
    cursor->insertText(headerText, charFormat2);

}

void ExportDialog::populateTable(QTextCursor *cursor)
{
    QTextDocument tableDocument;
    QTextCursor tableDocumentCursor(&tableDocument);

    QTextCursor tableCursor;
    QTextBlockFormat blockFormat;
    cursor->insertHtml("<hr>");

    QTextTable *table = tableDocumentCursor.insertTable(sqlModel->rowCount() + 1, sqlModel->columnCount(), tableFormat(TabFormat::Table));

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

    cursor->insertFragment(QTextDocumentFragment(&tableDocument));
}

void ExportDialog::populatePDF()
{
    QTextDocument document;
    document.setDefaultFont(QFont("Calibri", 9));
    QTextCursor textCursor(&document);

    addHeaderToDocument(&document,&textCursor);
    textCursor.movePosition(QTextCursor::NextRow);
    textCursor.movePosition(QTextCursor::NextBlock);
    populateTable(&textCursor);

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFileName(ui->pathLineEdit->text());
    printer.setOutputFormat(QPrinter::PdfFormat);
    document.print(&printer);

    QMessageBox::information( this, "Informacja", "Wyeksportowano do pliku:\n" + ui->pathLineEdit->text() );

    QDesktopServices::openUrl(ui->pathLineEdit->text());
}


