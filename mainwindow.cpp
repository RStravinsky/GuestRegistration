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

bool MainWindow::isConnectedToNetwork(){

    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    bool result = false;

    for (int i = 0; i < ifaces.count(); i++) {

        QNetworkInterface iface = ifaces.at(i);
        if ( iface.flags().testFlag(QNetworkInterface::IsUp)
             && !iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {

            for (int j=0; j<iface.addressEntries().count(); j++) {

                // got an interface which is up, and has an ip address
                if (result == false)
                    result = true;
            }
        }
    }
    return result;
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

    timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(on_timer_overflow()));
    timer->start(5000);
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
   if((login=="root" && password=="Serwis4q@") || (login=="solid" && password=="solidsigmasa")) {
       ui->addButton->setVisible(true);
       ui->deleteButton->setVisible(true);
       ui->line->setVisible(true);
   }
   else {
       ui->addButton->setVisible(false);
       ui->deleteButton->setVisible(false);
       ui->line->setVisible(false);
   }
   Statlabel->setText("<font color='white'>Połączono z użytkownikiem: <font color='green'>"+login+"</font></font>");
}

void MainWindow::on_mainButtonReleased(const QPushButton *mainButton)
{
    if( mainButton == ui->logoutButton )
        qApp->exit( MainWindow::EXIT_CODE_REBOOT );

    if( mainButton == ui->generateButton )
        //exportToPDF();
        populatePDF();
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
        if(sqlModel->insertRow(sqlModel->rowCount())) {
            isAdded = true;
            ui->tableView->scrollToBottom();
            isSubmit = !isSubmit;
            ui->addButton->setIcon(QIcon(":/images/images/submit.png"));
            ui->addButton->setStyleSheet("QPushButton {color: gray;border: 3px solid rgb(89,142,32);border-radius: 5px;background: rgb(35,35,35);}"
                                        "QPushButton:hover {color: white;border: 3px solid rgb(89,142,32);border-radius: 5px; background: qlineargradient"
                                        "(x1:0, y1:0, x2:0, y2:1,stop: 0 rgba(80,80,80), stop: 0.7 rgb(35,35,35));}"
                                        "QPushButton:pressed {color: white;border: 3px solid rgb(89,142,32);border-radius: 5px;background: rgb(80,80,80);}");
        }
        else
            QMessageBox::warning(this,"Informacja","Nie dodano osoby.");
    }
    else {
        if(submit(sqlModel)) {
        isSubmit = !isSubmit;
        isAdded = false;
        ui->addButton->setIcon(QIcon(":/images/images/add_person.png"));
        ui->addButton->setStyleSheet("QPushButton {color: gray;border: 2px solid rgb(20,20,20);border-radius: 5px;background: rgb(35,35,35);}"
                                    "QPushButton:hover {color: white;border: 2px solid rgb(20,20,20);border-radius: 5px; background: qlineargradient"
                                    "(x1:0, y1:0, x2:0, y2:1,stop: 0 rgba(80,80,80), stop: 0.7 rgb(35,35,35));}"
                                    "QPushButton:pressed {color: white;border: 2px solid rgb(20,20,20);border-radius: 5px;background: rgb(80,80,80);}");
        }
    }
}

bool MainWindow::submit(ModSqlTableModel *&model)
{
    if(!model->submitAll()) {
        QMessageBox::warning(this,"Informacja","Nie dodano osoby.\nPowód: "+model->lastError().text()+"");
        return false;
    }
    else {
        QMessageBox::information(this,"Informacja","Dodano osobę.");
        model->select();
        return true;
    }
}

QTextTableFormat MainWindow::tableFormat()
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

void MainWindow::addHeaderToDocument(QTextCursor *cursor, QString &title)
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

void MainWindow::populateTable(QTextCursor *cursor)
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

void MainWindow::populatePDF()
{
    QString pdfFile;
    QString filters( "Pliki PDF (*.pdf);;Wszystkie pliki (*.*)" );
    QString pdfFilter( "Pliki PDF (*.pdf)" );

    pdfFile = QFileDialog::getSaveFileName( this, "Wyeksportuj do pliku", QDir::homePath(), filters, &pdfFilter );


    QTextDocument document;
    document.setDefaultFont(QFont("Calibri", 9));
    QTextCursor textCursor(&document);
    QString headerText = "Wygenerowano: " + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss");

    addHeaderToDocument(&textCursor, headerText);
    populateTable(&textCursor);

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFileName(pdfFile);
    printer.setOutputFormat(QPrinter::PdfFormat);
    document.print(&printer);

    QMessageBox::information( this, "Informacja", "Wyeksportowano do pliku:\n" + pdfFile );

    QDesktopServices::openUrl(pdfFile);
}

void MainWindow::on_deleteButton_clicked()
{
    if(!isAdded) {
        QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
        if (!indexes.isEmpty()) {
            QSqlQuery query;
            query.prepare("call guestregistration.fillDepartureTime(:id)");
            query.bindValue(":id", ui->tableView->model()->index(ui->tableView->currentIndex().row(),0).data().toInt());
            if(!query.exec()) {
                QMessageBox::information(this,QString("Informacja"),QString("Polecenie nie powiodło się."));
                return;
            }
            sqlModel->select();
        }
        else
            QMessageBox::information(this,QString("Informacja"),QString("Nie zaznaczono wiersza."));
    }
    else
        QMessageBox::information(this,QString("Informacja"),QString("Nie można usunąć przed zatwierdzeniem."));
}

void MainWindow::on_timer_overflow()
{
    if(!isConnectedToNetwork()) {
        QMessageBox::critical(this,QString("Informacja"),QString("Połaczenie z bazą danych zostało przerwane!\nNastąpi przejście do okna logowania."));
        qApp->exit( MainWindow::EXIT_CODE_REBOOT );
    }
    timer->start(5000);
}
