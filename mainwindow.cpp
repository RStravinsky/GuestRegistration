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

    connect(this,SIGNAL(mainButtonReleased(const QPushButton*)),this,SLOT(on_mainButtonReleased(const QPushButton*)));

    timer = new QTimer(this);
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
    sqlModel->setFilter("ArrivalTime between DATE_SUB(CURDATE()+1,INTERVAL 10 DAY) And CURDATE()+1");
    sqlModel->sort(6,Qt::AscendingOrder);
    sqlModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    sqlModel->select();

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(sqlModel);
    ui->tableView->setModel(proxyModel);

    QSpreadsheetHeaderView * header = new QSpreadsheetHeaderView(Qt::Horizontal,this);
    ui->tableView->setHorizontalHeader(header);
    ui->tableView->show();

    configureTable();

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
   }
   else {
       ui->addButton->setVisible(false);
       ui->deleteButton->setVisible(false);;
   }
   Statlabel->setText("<font color='white'>Połączono z użytkownikiem: <b><font color='green'>"+login+"</font></b></font>");
}

void MainWindow::on_mainButtonReleased(const QPushButton *mainButton)
{
    if( mainButton == ui->logoutButton )
        qApp->exit( MainWindow::EXIT_CODE_REBOOT );

    if( mainButton == ui->generateButton ) {
        ExportDialog exportDialog(this);
        exportDialog.exec();
    }
}

void MainWindow::on_timer_overflow()
{
    if(!isConnectedToNetwork()) {
        QMessageBox::critical(this,QString("Błąd"),QString("Połaczenie z bazą danych zostało przerwane!\nNastąpi przejście do okna logowania."));
        qApp->exit( MainWindow::EXIT_CODE_REBOOT );
    }
    timer->start(5000);
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    proxyModel->setFilterKeyColumn(-1);
    QRegExp regExp(ui->lineEdit->text(), Qt::CaseInsensitive, QRegExp::Wildcard);
    proxyModel->setFilterRegExp(regExp);
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
            QMessageBox::warning(this,"Informacja","Nie dodano osoby.\nPrzyczyna: Nie można dodać nowego wiersza.");
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
    if(dataIsCorrect()) {
        if(!model->submitAll()) {
            QMessageBox::warning(this,"Informacja","Nie dodano osoby.\nPrzyczyna: "+model->lastError().text()+"");
            return false;
        }
        else {
            QMessageBox::information(this,"Informacja","Dodano osobę.");
            model->setFilter("ArrivalTime between DATE_SUB(CURDATE()+1,INTERVAL 10 DAY) And CURDATE()+1");
            model->select();
            return true;
        }
    }
    else
        return false;
}

void MainWindow::on_deleteButton_clicked()
{
    if(!isAdded) {
        QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
        if (!indexes.isEmpty()) {
            if(ui->tableView->model()->index(ui->tableView->currentIndex().row(),7).data().toString().isEmpty()) {
                QSqlQuery query;
                query.prepare("call guestregistration.fillDepartureTime(:id)");
                query.bindValue(":id", ui->tableView->model()->index(ui->tableView->currentIndex().row(),0).data().toInt());
                if(!query.exec()) {
                    QMessageBox::information(this,QString("Informacja"),QString("Polecenie nie powidoło się."));
                    return;
                }
                sqlModel->setFilter("ArrivalTime between DATE_SUB(CURDATE()+1,INTERVAL 10 DAY) And CURDATE()+1");
                sqlModel->select();
            }
            else
                QMessageBox::information(this,QString("Informacja"),QString("Osoba już wyszła."));
        }
        else
            QMessageBox::information(this,QString("Informacja"),QString("Nie zaznaczono wiersza."));
    }
    else
        QMessageBox::information(this,QString("Informacja"),QString("Nie można usunąć przed zatwierdzeniem."));
}

bool MainWindow::dataIsCorrect()
{
    QString name = sqlModel->index(sqlModel->rowCount()-1,1).data().toString();
    QString surename = sqlModel->index(sqlModel->rowCount()-1,2).data().toString();

    if(sqlModel->index(sqlModel->rowCount()-1,1).data().toString().isEmpty() ||
       sqlModel->index(sqlModel->rowCount()-1,2).data().toString().isEmpty()) {
        QMessageBox::information(this,QString("Informacja"),QString("Nie dodano osoby.\nPrzyczyna: Imię oraz nazwisko musi być uzupełnione."));
        return false;
    }
    if(name.contains(" ") || surename.contains(" ")) {
         QMessageBox::information(this,QString("Informacja"),QString("Nie dodano osoby.\nPrzyczyna: Imię lub nazwisko zawiera pusty znak (" ")."));
         return false;

    }
    return true;
}




void MainWindow::on_sigmaButton_clicked()
{
    static bool isSigma = false;
    isSigma = !isSigma;

    if(isSigma)
        ui->sigmaButton->setIcon(QIcon(":/images/images/onlySigma.png"));
    if(!isSigma)
        ui->sigmaButton->setIcon(QIcon(":/images/images/onlySigmaChecked.png"));
}
