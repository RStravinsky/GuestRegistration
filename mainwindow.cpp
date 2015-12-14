#include "mainwindow.h"
#include "ui_mainwindow.h"

extern double dpiPercent;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    addStatusBar();
    ui->logoutButton->installEventFilter(this);
    ui->helpButton->installEventFilter(this);
    ui->generateButton->installEventFilter(this);
    ui->tableView->installEventFilter(this);
    connect(this,SIGNAL(mainButtonReleased(const QPushButton*)),this,SLOT(on_mainButtonReleased(const QPushButton*)));
}

MainWindow::~MainWindow()
{
    LoginDialog loginDialog;
    loginDialog.closeDatabase();
    delete ui;
}

void MainWindow::loadSqlModel()
{
    sqlModel = new ModSqlTableModel(this);
    sqlModel->setTable("registration");
    sqlModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    sqlModel->select();
    ui->tableView->setModel(sqlModel);
    ui->tableView->show();

    configureTable();
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    QPushButton * actualButton = qobject_cast<QPushButton *>(obj);

    if (event->type() == QEvent::HoverEnter) {
        actualButton->setIconSize(QSize(100, 100));
        return true;
    }
    else if (event->type() == QEvent::HoverLeave) {
        actualButton->setIconSize(QSize(90, 90));
        return true;
    }

    else if (event->type() == QEvent::MouseButtonRelease) {
        actualButton->setIconSize(QSize(100, 100));
        emit mainButtonReleased(actualButton);
        return true;
    }

    else if (event->type() == QEvent::MouseButtonPress) {
        actualButton->setIconSize(QSize(80, 80));
        return true;
    }

    return false;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    if(event->type() == QEvent::Resize) {
        if(this->width()>1920) this->setMaximumWidth(1920);
        if(this->height()>1080) this->setMaximumHeight(1080);
    }
}

void MainWindow::on_sendAccess(QString login,QString password)
{
   Statprogress->setValue(100);
   if(login=="root" && password=="Serwis4q@")
   {
       readOnly=true;
       Statlabel->setText("<font color='white'>Połączono z bazą danych</font>");
   }
}

void MainWindow::on_mainButtonReleased(const QPushButton *mainButton)
{
    if( mainButton == ui->logoutButton ) {
        qDebug() << "Performing application reboot...";
        qApp->exit( MainWindow::EXIT_CODE_REBOOT );
    }

    if( mainButton == ui->generateButton ) {
        exportToPDF();
    }
}

void MainWindow::addStatusBar()
{
    QFont Status_Font("Calibri Light", 12, QFont::Light ,false);
    Statprogress = new QProgressBar(this);
    Statlabel = new QLabel(this);
    Statlabel->setFont(Status_Font);
    Statlabel->setStyleSheet("background: transparent");
    Statprogress->setStyleSheet("QProgressBar {border: 2px solid rgb(20,20,20); border-radius: 5px;background: rgb(20,20,20);min-height: 20px}"
    "QProgressBar::chunk {background: #088E8C ;border-radius: 4px;}");
    Statprogress->setTextVisible(false);
    ui->statusBar->addWidget(Statlabel,1);
    ui->statusBar->addPermanentWidget(Statprogress,1);
    ui->statusBar->setFont(Status_Font);
    ui->statusBar->setStyleSheet("QStatusBar {background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                                 "stop: 0 rgba(35,35,35), stop: 0.7 rgb(80,80,80));;min-height: 30px}");
}

void MainWindow::configureTable()
{
    QFont Font("Calibri Light", 14, QFont::Light ,false);

    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->verticalHeader()->setFixedWidth(30);
    ui->tableView->horizontalHeader()->setFixedHeight(30);
    ui->tableView->hideColumn(0);
    QStringList headerList ({"Imię", "Nazwisko", "Firma", "Tablica rejestracyjna", "Cel wizyty", "Czas przyjazdu", "Czas wyjazdu"});
    for(int i=1; i<sqlModel->columnCount(); ++i) {
        ui->tableView->setColumnWidth(i,266);
        sqlModel->setHeaderData(i, Qt::Horizontal, headerList.at(i-1));
    };

    ui->tableView->horizontalHeader()->setFont(Font);
    ui->tableView->setFont(Font);
}


void MainWindow::on_addButton_clicked()
{
    static bool isSubmit = false;
    if(!isSubmit) {
        ui->addButton->setIcon(QIcon(":/images/images/submit.png"));
        ui->addButton->setStyleSheet("QPushButton {color: gray;border: 3px solid rgb(89,142,32);border-radius: 5px;background: rgb(35,35,35);}"
                                    "QPushButton:hover {color: white;border: 3px solid rgb(89,142,32);border-radius: 5px; background: qlineargradient"
                                    "(x1:0, y1:0, x2:0, y2:1,stop: 0 rgba(80,80,80), stop: 0.7 rgb(35,35,35));}"
                                    "QPushButton:pressed {color: white;border: 3px solid rgb(89,142,32);border-radius: 5px;background: rgb(80,80,80);}");
        isSubmit = !isSubmit;
        ui->tableView->scrollToBottom();
        sqlModel->insertRow(sqlModel->rowCount());
    }
    else {
        ui->addButton->setIcon(QIcon(":/images/images/add_person.png"));
        ui->addButton->setStyleSheet("QPushButton {color: gray;border: 2px solid rgb(20,20,20);border-radius: 5px;background: rgb(35,35,35);}"
                                    "QPushButton:hover {color: white;border: 2px solid rgb(20,20,20);border-radius: 5px; background: qlineargradient"
                                    "(x1:0, y1:0, x2:0, y2:1,stop: 0 rgba(80,80,80), stop: 0.7 rgb(35,35,35));}"
                                    "QPushButton:pressed {color: white;border: 2px solid rgb(20,20,20);border-radius: 5px;background: rgb(80,80,80);}");
        isSubmit = !isSubmit;
        submit(sqlModel);
    }


}

void MainWindow::submit(ModSqlTableModel *&model)
{
    if(!model->submitAll())
    {
        QMessageBox::warning(this,"BŁĄD",model->lastError().text());
        model->select();
    }
    else
        QMessageBox::information(this,"Informacja","Dodano.");

}

void MainWindow::on_deleteButton_clicked()
{
    QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
    if (!indexes.isEmpty()) {

        if(sqlModel->index(ui->tableView->currentIndex().row(),7).data().isNull()) {
            QSqlQuery query;
            query.prepare("call guestregistration.fillDepartureTime(:id)");
            qDebug() << ui->tableView->model()->index(ui->tableView->currentIndex().row(),0).data().toInt();
            query.bindValue(":id", ui->tableView->model()->index(ui->tableView->currentIndex().row(),0).data().toInt());
            if(!query.exec())
                QMessageBox::information(this,QString("Informacja"),QString("Polecenie nie powiodło się."));
            sqlModel->select();
        }
        else {
            QMessageBox::information(this,QString("Informacja"),QString("Osoba już wyszła!"));
        }

    }

    else
        QMessageBox::information(this,QString("Informacja"),QString("Nie zaznaczono wiersza."));
}


QPixmap MainWindow::grabTable()
{
    const int rows = ui -> tableView -> model() -> rowCount();
    const int columns = ui -> tableView -> model() -> columnCount();

    double totalWidth = ui -> tableView -> verticalHeader() -> width();
    for ( int c = 0; c < columns; ++c )
        totalWidth += ui -> tableView -> columnWidth(c);

    double totalHeight = ui -> tableView -> horizontalHeader() -> height();
    for ( int r = 0; r < rows; ++r )
        totalHeight += ui -> tableView -> rowHeight(r);

    QTableView tempTable( ui->tableView );
    tempTable.setFixedHeight( totalHeight );
    tempTable.setModel( ui -> tableView -> model() );
    tempTable.setFixedSize( totalWidth, totalHeight );
    tempTable.setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    tempTable.setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    tempTable.verticalHeader() -> hide();
    tempTable.horizontalHeader()->setStyleSheet("font: 22px;");


    for ( int i = 0; i < ui -> tableView -> model() -> columnCount() ; ++i )
        tempTable.setColumnWidth( i, ui -> tableView -> columnWidth(0) );
    tempTable.horizontalHeader() -> setStretchLastSection( true );

    QPixmap grabPixmap = QPixmap::grabWidget( &tempTable );
    return grabPixmap;

}

void MainWindow::exportToPDF()
{
    QPrinter printer( QPrinter::HighResolution );
    QString pdfFile;
    QString filters( "Pliki PDF (*.pdf);;Wszystkie pliki (*.*)" );
    QString pdfFilter( "Pliki PDF (*.pdf)" );

    pdfFile = QFileDialog::getSaveFileName( this, "Wyeksportuj do pliku", QDir::homePath(), filters, &pdfFilter );

    if (pdfFile.isEmpty()) return;

    printer.setOutputFileName( pdfFile );
    printer.setOutputFormat( QPrinter::PdfFormat );
    printer.setFullPage( true );
    printer.setPageMargins( 15, 0, 15, 0, QPrinter::Millimeter );


    QPainter painter( &printer );
    painter.scale( 8, 8 );

    QPixmap pixmap = grabTable();

    QRectF sourceRect;
    double totalPageHeight = ui -> tableView -> horizontalHeader() -> height();
    int columnCount = 0;
    int rowCount = 0;
    int pageCount = 1;

    QFont headerFont;
    headerFont.setFamily("Calibri");
    headerFont.setPixelSize(1200/dpiPercent);
    headerFont.setWeight(1200/dpiPercent);
    painter.setFont( headerFont );

    QPoint offsetPdf, offsetDirect;
    offsetPdf.setX( 50 );
    offsetPdf.setY( 100 );
    offsetDirect.setX( -50 );
    offsetDirect.setY( 100 );

    // First take the rows that fit into one page
    for ( int h = 0; h < ui -> tableView -> model() -> rowCount(); h++ )
    {
        totalPageHeight += ui -> tableView -> rowHeight(h);
        double totalPageWidth = ui -> tableView -> verticalHeader() -> width();

        if ( rowCount == 49 || h == ui -> tableView -> model() -> rowCount() - 1 )
        {
            // Then take the columns that fit into one page
            for ( int w = 0; w < ui -> tableView -> model() -> columnCount(); w++ )
            {
                totalPageWidth += ui -> tableView -> columnWidth(w);
                if ( columnCount == 7 || (w == ui -> tableView -> model() -> columnCount() - 1) )
                {
                    sourceRect.setWidth( totalPageWidth );
                    sourceRect.setHeight( totalPageHeight + 14 );

                    if( pageCount == 1 )
                    {
                        painter.drawText( QPoint(50, 50), "Wygenerowano: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm") );
                        //painter.drawText( QPoint(50 + ui -> tableView -> columnWidth(0), 50), "Od: " + ui -> dateTimeStart -> dateTime().toString("yyyy-MM-dd hh") );
                        //painter.drawText( QPoint(50 + ui -> tableView -> columnWidth(0), 75), "Do: " + ui -> dateTimeEnd -> dateTime().toString("yyyy-MM-dd hh") );
                    }
                    painter.drawPixmap( printer.pageRect().topLeft()+offsetPdf, pixmap, sourceRect );
                    sourceRect.setLeft( sourceRect.left() + totalPageWidth );

                    if ( w != ui -> tableView -> model() -> columnCount() - 1 )
                        printer.newPage();

                    totalPageWidth = 0;
                    columnCount = 0;
                }
                else
                {
                    columnCount++;
                }
            }


            sourceRect.setTop( sourceRect.top() + totalPageHeight + 7 );
            sourceRect.setLeft( 0 );

            if (h != ui -> tableView -> model() -> rowCount() - 1)
            {
                ++pageCount;
                printer.newPage();
            }

            totalPageHeight = 0;
            rowCount = 0;
        }
        else
        {
            rowCount++;
        }
    }

    QMessageBox::information( this, "Informacja", "Wyeksportowano do pliku:\n" + pdfFile );
}
