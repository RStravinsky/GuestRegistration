#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    sqlModel = new QSqlTableModel(this);
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

void MainWindow::submit(QSqlTableModel *&model)
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
        QSqlQuery query;
        query.prepare("call guestregistration.fillDepartureTime(:id)");
        qDebug() << ui->tableView->model()->index(ui->tableView->currentIndex().row(),0).data().toInt();
        query.bindValue(":id", ui->tableView->model()->index(ui->tableView->currentIndex().row(),0).data().toInt());
        if(!query.exec())
            QMessageBox::information(this,QString("Informacja"),QString("Polecenie nie powidoło się."));
        sqlModel->select();
    }

    else
        QMessageBox::information(this,QString("Informacja"),QString("Nie zaznaczono wiersza."));
}


void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    QString headerName=ui->tableView->model()->headerData(ui->tableView->currentIndex().column(), Qt::Horizontal).toString();
    if(headerName.contains("Czas przyjazdu") || headerName.contains("Czas wyjazdu"))
        QMessageBox::information(this,QString("Informacja"),QString("Pole uzupełniane jest automatycznie."));
}
