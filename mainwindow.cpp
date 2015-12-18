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
    timer = new QTimer(this);

    loadTrayIcon();
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
    sqlModel->setFilter("ArrivalTime between DATE_SUB(CURDATE()+1,INTERVAL 30 DAY) And CURDATE()+1");
    sqlModel->sort(6,Qt::AscendingOrder);
    sqlModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    sqlModel->select();

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(sqlModel);
    ui->tableView->setModel(proxyModel);

    configureTable();
    ui->tableView->show();

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

//    if (event->type() == QEvent::KeyPress)
//       {
//        if(obj == (QObject*)ui->tableView)
//           {
//               QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
//               if(keyEvent->key() == Qt::Key_Return)
//               {

//               }
//           }
//       }

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
       ui->addGroupButton->setVisible(true);
       ui->deleteButton->setVisible(true);
       ui->deleteGroup->setVisible(true);
   }
   else {
       login = QString("sigmasa");
       ui->addButton->setVisible(false);
       ui->deleteButton->setVisible(false);
       ui->addGroupButton->setVisible(false);
       ui->deleteGroup->setVisible(false);
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

    if( mainButton == ui->helpButton ) {
        HelpDialog helpDialog(this);
        helpDialog.exec();
    }

}

void MainWindow::on_timer_overflow()
{
    if(!isConnectedToNetwork()) {
        QMessageBox::critical(this,QString("Błąd"),QString("Połaczenie z bazą danych zostało przerwane!\nNastąpi przejście do okna logowania."));
        qApp->exit( MainWindow::EXIT_CODE_REBOOT );
    }

    if(Statlabel->text().contains("sigmasa"))
            setPopupMessage();

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

    QSpreadsheetHeaderView * header = new QSpreadsheetHeaderView(Qt::Horizontal,this);
    ui->tableView->setHorizontalHeader(header);
    ui->tableView->horizontalHeader()->setFixedHeight(30);
    QStringList headerList ({"Imię", "Nazwisko", "Firma", "Tablica rejestracyjna", "Cel wizyty", "Czas przyjazdu", "Czas wyjazdu"});
    for(int i=1; i<sqlModel->columnCount(); ++i) {
        ui->tableView->setColumnWidth(i,266);
        sqlModel->setHeaderData(i, Qt::Horizontal, headerList.at(i-1));
    };
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setFont(Font);
    ui->tableView->horizontalHeader()->setVisible(true);

    ui->tableView->verticalHeader()->setFixedWidth(30);
    ui->tableView->hideColumn(0);
    ui->tableView->setFont(Font);
    ui->tableView->scrollToBottom();
}

QString MainWindow::setButtonsStyleSheet(ButtonStyle style)
{
    if(style == ButtonStyle::Submit)
        return QString("QPushButton:hover {color: gray;border: 3px solid rgb(89,142,32);border-radius: 5px;background: rgb(35,35,35);}"
                        "QPushButton {color: white;border: 3px solid rgb(89,142,32);border-radius: 5px; background: qlineargradient"
                        "(x1:0, y1:0, x2:0, y2:1,stop: 0 rgba(80,80,80), stop: 0.7 rgb(35,35,35));}"
                        "QPushButton:pressed {color: white;border: 3px solid rgb(89,142,32);border-radius: 5px;background: rgb(80,80,80);}");
    else if(style == ButtonStyle::Remove)
        return QString("QPushButton:hover {color: gray;border: 3px solid rgb(164,46,46);border-radius: 5px;background: rgb(35,35,35);}"
                       "QPushButton {color: white;border: 3px solid rgb(164,46,46);border-radius: 5px; background: qlineargradient"
                       "(x1:0, y1:0, x2:0, y2:1,stop: 0 rgba(80,80,80), stop: 0.7 rgb(35,35,35));}"
                       "QPushButton:pressed {color: white;border: 3px solid rgb(164,46,46);border-radius: 5px;background: rgb(80,80,80);}");
    else if(style == ButtonStyle::Normal)
        return QString("QPushButton:hover {color: gray;border: 2px solid rgb(20,20,20);border-radius: 5px;background: rgb(35,35,35);}"
                       "QPushButton {color: white;border: 2px solid rgb(20,20,20);border-radius: 5px; background: qlineargradient"
                       "(x1:0, y1:0, x2:0, y2:1,stop: 0 rgba(80,80,80), stop: 0.7 rgb(35,35,35));}"
                       "QPushButton:pressed {color: white;border: 2px solid rgb(20,20,20);border-radius: 5px;background: rgb(80,80,80);}");

    else if(style == ButtonStyle::SwitchRightON)
        return QString("QPushButton {"
                       "color: white;"
                       "border-left: 2px solid rgb(20,20,20);"
                       "border-top: 2px solid rgb(20,20,20);"
                       "border-bottom: 2px solid rgb(20,20,20);"
                       "background: rgb(35,35,35);"
                       "border-bottom-left-radius: 5px;"
                       "border-top-left-radius:5px;}"
                       "QPushButton:pressed {"
                       "color: white;"
                       "border-left: 2px solid rgb(20,20,20);"
                       "border-top: 2px solid rgb(20,20,20);"
                       "border-bottom: 2px solid rgb(20,20,20);"
                       "border-bottom-left-radius: 5px;"
                       "border-top-left-radius:5px;"
                       "background: rgb(80,80,80);}");

    else if(style == ButtonStyle::SwitchRightOFF)
        return QString("QPushButton {"
                       "color: white;"
                       "border-left: 2px solid rgb(20,20,20);"
                       "border-top: 2px solid rgb(20,20,20);"
                       "border-bottom: 2px solid rgb(20,20,20);"
                       "background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                        "stop: 0 rgba(80,80,80), stop: 0.7 rgb(50,50,50));"
                       "border-bottom-left-radius: 5px;"
                       "border-top-left-radius:5px;}"
                       "QPushButton:pressed {"
                       "color: white;"
                       "border-left: 2px solid rgb(20,20,20);"
                       "border-top: 2px solid rgb(20,20,20);"
                       "border-bottom: 2px solid rgb(20,20,20);"
                       "border-bottom-left-radius: 5px;"
                       "border-top-left-radius:5px;"
                       "background: rgb(80,80,80);}");

    else if(style == ButtonStyle::SwitchLeftON)
        return QString("QPushButton {"
                       "color: white;"
                       "border-right: 2px solid rgb(20,20,20);"
                       "border-top: 2px solid rgb(20,20,20);"
                       "border-bottom: 2px solid rgb(20,20,20);"
                       "background: rgb(35,35,35);"
                       "border-bottom-right-radius: 5px;"
                       "border-top-right-radius:5px;}"
                       "QPushButton:pressed {"
                       "color: white;"
                       "border-right: 2px solid rgb(20,20,20);"
                       "border-top: 2px solid rgb(20,20,20);"
                       "border-bottom: 2px solid rgb(20,20,20);"
                       "border-bottom-right-radius: 5px;"
                       "border-top-right-radius:5px;"
                       "background: rgb(80,80,80);}");

    else if(style == ButtonStyle::SwitchLeftOFF)
        return QString("QPushButton {"
                       "color: white;"
                       "border-right: 2px solid rgb(20,20,20);"
                       "border-top: 2px solid rgb(20,20,20);"
                       "border-bottom: 2px solid rgb(20,20,20);"
                       "background:  qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                        "stop: 0 rgba(80,80,80), stop: 0.7 rgb(50,50,50));"
                       "border-bottom-right-radius: 5px;"
                       "border-top-right-radius:5px;}"
                       "QPushButton:pressed {"
                       "color: white;"
                       "border-right: 2px solid rgb(20,20,20);"
                       "border-top: 2px solid rgb(20,20,20);"
                       "border-bottom: 2px solid rgb(20,20,20);"
                       "border-bottom-right-radius: 5px;"
                       "border-top-right-radius:5px;"
                       "background: rgb(80,80,80);}");

    return QString("");
}

bool MainWindow::dataIsCorrect()
{
    if(ModSqlTableModel::isGroup == true) {
        if(sqlModel->index(sqlModel->rowCount()-1,3).data().toString().isEmpty()) {
            QMessageBox::information(this,QString("Informacja"),QString("Nie dodano grupy.\nPrzyczyna: Firma musi być uzupełniona."));
            return false;
        }
        return true;
    }

    else {
        QString name = sqlModel->index(sqlModel->rowCount()-1,1).data().toString();
        QString surename = sqlModel->index(sqlModel->rowCount()-1,2).data().toString();

        if(sqlModel->index(sqlModel->rowCount()-1,1).data().toString().isEmpty() ||
           sqlModel->index(sqlModel->rowCount()-1,2).data().toString().isEmpty()) {
            QMessageBox::information(this,QString("Informacja"),QString("Nie dodano osoby.\nPrzyczyna: Imię oraz nazwisko musi być uzupełnione."));
            return false;
        }
        if(name.contains(" ") || surename.contains(" ")) {
             QMessageBox::information(this,QString("Informacja"),QString("Nie dodano osoby.\nPrzyczyna: Imię lub nazwisko zawiera pusty znak (\" \")."));
             return false;

        }
        return true;
    }
}

bool MainWindow::submit(ModSqlTableModel *&model)
{
    if(dataIsCorrect()) {
        if(!model->submitAll()) {
            if(!ModSqlTableModel::isGroup)
                QMessageBox::warning(this,"Informacja","Nie dodano osoby.\nPrzyczyna: "+model->lastError().text()+"");
            else {
                QMessageBox::warning(this,"Informacja","Nie dodano grupy.\nPrzyczyna: "+model->lastError().text()+"");
                ModSqlTableModel::isGroup = false;
            }
            return false;
        }
        else {
            if(!ModSqlTableModel::isGroup)
                QMessageBox::information(this,"Informacja","Dodano osobę.");
            else {
                QMessageBox::information(this,"Informacja","Dodano grupę.");
                ModSqlTableModel::isGroup = false;
            }
            if(isSigmaFilter) sqlModel->setFilter("DepartureTime IS NULL");
            else model->setFilter("ArrivalTime between DATE_SUB(CURDATE()+1,INTERVAL 30 DAY) And CURDATE()+1");
            model->select();
            return true;
        }
    }
    else
        return false;
}

void MainWindow::on_addButton_clicked()
{
    if(!isPersonAdded) {
        if(!sqlModel->isDirty()) {
            if(sqlModel->insertRow(sqlModel->rowCount())) {
                ui->tableView->scrollToBottom();
                isPersonAdded = !isPersonAdded;
                ui->addButton->setIcon(QIcon(":/images/images/submit.png"));
                ui->addButton->setStyleSheet(setButtonsStyleSheet(ButtonStyle::Submit));
                ui->deleteButton->setIcon(QIcon(":/images/images/remove.png"));
                ui->deleteButton->setStyleSheet(setButtonsStyleSheet(ButtonStyle::Remove));
            }
            else
                QMessageBox::warning(this,"Informacja","Nie dodano osoby.\nPrzyczyna: Nie można dodać nowego wiersza.");
        }
        else
            QMessageBox::information(this,QString("Informacja"),QString("Nie zatwierdzono."));
    }
    else {
        if(submit(sqlModel)) {
        isPersonAdded = !isPersonAdded;
        ui->addButton->setIcon(QIcon(":/images/images/add_person.png"));
        ui->addButton->setStyleSheet(setButtonsStyleSheet(ButtonStyle::Normal));
        ui->deleteButton->setIcon(QIcon(":/images/images/delete_person.png"));
        ui->deleteButton->setStyleSheet(setButtonsStyleSheet(ButtonStyle::Normal));
        }
    }
}

void MainWindow::on_deleteButton_clicked()
{
    if(!sqlModel->isDirty()) {
            QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
            if (!indexes.isEmpty()) {
                QString firstRow = ui->tableView->model()->index(ui->tableView->currentIndex().row(),1).data().toString();
                if(firstRow!=QString("-----")) {
                    if(ui->tableView->model()->index(ui->tableView->currentIndex().row(),7).data().toString().isEmpty()) {
                            QSqlQuery query;
                            query.prepare("call guestregistration.fillDepartureTime(:id)");
                            query.bindValue(":id", ui->tableView->model()->index(ui->tableView->currentIndex().row(),0).data().toInt());
                            if(!query.exec()) {
                                QMessageBox::information(this,QString("Informacja"),QString("Polecenie nie powidoło się."));
                                return;
                            }
                            if(isSigmaFilter) sqlModel->setFilter("DepartureTime IS NULL");
                            else sqlModel->setFilter("ArrivalTime between DATE_SUB(CURDATE()+1,INTERVAL 30 DAY) And CURDATE()+1");
                            sqlModel->select();
                            }
                    else
                        QMessageBox::information(this,QString("Informacja"),QString("Osoba już wyszła."));
                }
                else
                    QMessageBox::information(this,QString("Informacja"),QString("Przycisk dotyczy osoby a nie grupy."));
            }
            else
                QMessageBox::information(this,QString("Informacja"),QString("Nie zaznaczono wiersza."));
    }
    else if(!isGroupAdded){
        sqlModel->select();
        isPersonAdded = !isPersonAdded;
        ui->addButton->setIcon(QIcon(":/images/images/add_person.png"));
        ui->addButton->setStyleSheet(setButtonsStyleSheet(ButtonStyle::Normal));
        ui->deleteButton->setIcon(QIcon(":/images/images/delete_person.png"));
        ui->deleteButton->setStyleSheet(setButtonsStyleSheet(ButtonStyle::Normal));
    }
    else
        QMessageBox::information(this,QString("Informacja"),QString("Nie zatwierdzono."));
}

void MainWindow::on_addGroupButton_clicked()
{
        if(!isGroupAdded) {
            if(!sqlModel->isDirty()) {
                if(sqlModel->insertRow(sqlModel->rowCount())) {
                    ui->tableView->scrollToBottom();
                    isGroupAdded = !isGroupAdded;
                    ui->addGroupButton->setIcon(QIcon(":/images/images/submit.png"));
                    ui->addGroupButton->setStyleSheet(setButtonsStyleSheet(ButtonStyle::Submit));
                    ui->deleteGroup->setIcon(QIcon(":/images/images/remove.png"));
                    ui->deleteGroup->setStyleSheet(setButtonsStyleSheet(ButtonStyle::Remove));

                    sqlModel->setData(sqlModel->index(sqlModel->rowCount()-1,1), "-----");
                    sqlModel->setData(sqlModel->index(sqlModel->rowCount()-1,2), "-----");
                    ModSqlTableModel::isGroup = true;
                }
                else
                    QMessageBox::warning(this,"Informacja","Nie dodano grupy.\nPrzyczyna: Nie można dodać nowego wiersza.");
            }
            else
                QMessageBox::information(this,QString("Informacja"),QString("Nie zatwierdzono."));
        }
        else {
            if(submit(sqlModel)) {
            isGroupAdded = !isGroupAdded;
            ui->addGroupButton->setIcon(QIcon(":/images/images/add_group.png"));
            ui->addGroupButton->setStyleSheet(setButtonsStyleSheet(ButtonStyle::Normal));
            ui->deleteGroup->setIcon(QIcon(":/images/images/delete_group.png"));
            ui->deleteGroup->setStyleSheet(setButtonsStyleSheet(ButtonStyle::Normal));
            }
        }
}

void MainWindow::on_deleteGroup_clicked()
{
    if(!sqlModel->isDirty()) {
            QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
            if (!indexes.isEmpty()) {
                QString firstRow = ui->tableView->model()->index(ui->tableView->currentIndex().row(),1).data().toString();
                if(firstRow==QString("-----")) {
                    if(ui->tableView->model()->index(ui->tableView->currentIndex().row(),7).data().toString().isEmpty()) {
                            QSqlQuery query;
                            query.prepare("call guestregistration.fillDepartureTime(:id)");
                            query.bindValue(":id", ui->tableView->model()->index(ui->tableView->currentIndex().row(),0).data().toInt());
                            if(!query.exec()) {
                                QMessageBox::information(this,QString("Informacja"),QString("Polecenie nie powidoło się."));
                                return;
                            }
                            if(isSigmaFilter) sqlModel->setFilter("DepartureTime IS NULL");
                            else sqlModel->setFilter("ArrivalTime between DATE_SUB(CURDATE()+1,INTERVAL 30 DAY) And CURDATE()+1");
                            sqlModel->select();
                        }
                    else
                        QMessageBox::information(this,QString("Informacja"),QString("Grupa już wyszła."));
                }
                else
                    QMessageBox::information(this,QString("Informacja"),QString("Przycisk dotyczy grupy a nie osoby."));
            }
            else
                QMessageBox::information(this,QString("Informacja"),QString("Nie zaznaczono wiersza."));
    }
    else if(!isPersonAdded){
        sqlModel->select();
        isGroupAdded = !isGroupAdded;
        ModSqlTableModel::isGroup = false;
        ui->addGroupButton->setIcon(QIcon(":/images/images/add_group.png"));
        ui->addGroupButton->setStyleSheet(setButtonsStyleSheet(ButtonStyle::Normal));
        ui->deleteGroup->setIcon(QIcon(":/images/images/delete_group.png"));
        ui->deleteGroup->setStyleSheet(setButtonsStyleSheet(ButtonStyle::Normal));
    }
    else
        QMessageBox::information(this,QString("Informacja"),QString("Nie zatwierdzono."));
}

void MainWindow::on_sigmaButton_clicked()
{
     if(!sqlModel->isDirty()) {
         isSigmaFilter = true;
         ui->sigmaButton->setIconSize(QSize(35,35));
         ui->sigmaButton->setStyleSheet(setButtonsStyleSheet(ButtonStyle::SwitchRightON));
         ui->othersButton->setIconSize(QSize(16,16));
         ui->othersButton->setStyleSheet(setButtonsStyleSheet(ButtonStyle::SwitchLeftOFF));
         sqlModel->setFilter("DepartureTime IS NULL");
         sqlModel->select();
         ui->tableView->scrollToBottom();
     }
     else QMessageBox::information(this,QString("Informacja"),QString("Nie zatwierdzono."));
}

void MainWindow::on_othersButton_clicked()
{
    if(!sqlModel->isDirty()) {
        isSigmaFilter = false;
        ui->othersButton->setIconSize(QSize(35,35));
        ui->othersButton->setStyleSheet(setButtonsStyleSheet(ButtonStyle::SwitchLeftON));
        ui->sigmaButton->setIconSize(QSize(16,16));
        ui->sigmaButton->setStyleSheet(setButtonsStyleSheet(ButtonStyle::SwitchRightOFF));
        sqlModel->setFilter("ArrivalTime between DATE_SUB(CURDATE()+1,INTERVAL 30 DAY) And CURDATE()+1");
        sqlModel->select();
        ui->tableView->scrollToBottom();
    }
    else QMessageBox::information(this,QString("Informacja"),QString("Nie zatwierdzono."));
}

void MainWindow::setIcon()
{
    trayIcon->setIcon(QIcon(":/images/images/icon.ico"));
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
            break;
    case QSystemTrayIcon::DoubleClick:
            dcAction->activate(QAction::Trigger);
            break;
    case QSystemTrayIcon::MiddleClick:
            break;
    default:
        ;
    }
}

void MainWindow::on_editFinished(QWidget *, QAbstractItemDelegate::EndEditHint)
{
  qDebug() << "KUPA" << endl;
}

void MainWindow::createActions()
{
    QBrush wBrush;
    wBrush.setColor(Qt::white);
    QFont wFont;
    wFont.setFamily("Segou UI");
    wFont.setPointSize(9);


    minimizeAction = new QAction(tr("Mi&nimalizuj"), this);
    minimizeAction->setFont(QFont("Segou UI", 9));
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    restoreAction = new QAction(tr("&Przywróć"), this);
    restoreAction->setFont(QFont("Segou UI", 9));
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(show()));

    quitAction = new QAction(tr("&Zamknij"), this);
    quitAction->setFont(QFont("Segou UI", 9));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->setStyleSheet("QMenu {background: rgb(200,200,200);}");
    minimizeAction->setIcon(QIcon(":/images/images/minimize.png"));
    trayIconMenu->addAction(minimizeAction);
    restoreAction->setIcon(QIcon(":/images/images/restore.png"));
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    quitAction->setIcon(QIcon(":/images/images/close.png"));
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {

        QMessageBox msgBox(QMessageBox::Question, tr("Ewidencja gości"), tr("<font face=""Calibri Light"" size=""5"" color=""white""><b>Czy chcesz zminimalizować program do paska zadań?</b></font>"), QMessageBox::Yes | QMessageBox::No );

        msgBox.setStyleSheet("QMessageBox {background: black;}"
                             "QPushButton {"
                             "color: gray;"
                             "border: 2px solid rgb(20,20,20);"
                             "border-radius: 5px;"
                             "background: rgb(35,35,35);"
                             "width: 100;"
                             "height: 40;"
                             "font-family: Calibri Light;"
                             "font-size: 12;"
                             "font-weight: bold;"
                             "}"
                             "QPushButton:hover {"
                             "color: white;"
                             "border: 2px solid rgb(20,20,20);"
                             "border-radius: 5px;"
                             "background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                                "stop: 0 rgba(80,80,80), stop: 0.7 rgb(35,35,35));"
                             "}"
                             "QPushButton:pressed {"
                             "color: white;"
                             "border: 2px solid rgb(20,20,20);"
                             "border-radius: 5px;"
                             "background: rgb(80,80,80);"
                             "}"

                             );

        msgBox.setWindowFlags(Qt::FramelessWindowHint);
        msgBox.setWindowIcon(QIcon(":/images/images/icon.ico"));
        msgBox.setButtonText(QMessageBox::Yes, tr("Tak"));
        msgBox.setButtonText(QMessageBox::No, tr("Nie"));
        if (msgBox.exec() == QMessageBox::No) {
            QApplication::quit();
        }
        hide();
        event->ignore();
    }
}

void MainWindow::showMessage()
{
    QString title = "Wiadomość";
    QString msg = "Treść przykładowej wiadomości";
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information;
    trayIcon->showMessage(title, msg, icon, 5000);
}

void MainWindow::setPopupMessage()
{
    int actualRowCount = sqlModel->rowCount();
    sqlModel->select();
    if( sqlModel->rowCount() > actualRowCount && (!sqlModel->isDirty()) ) {
        actualRowCount = sqlModel->rowCount();

        QString firstCell = sqlModel->index(sqlModel->rowCount()-1,1).data().toString();
        if(firstCell!=QString("-----")) {

                QString title = "Nowa osoba w firmie:";
                QString company = sqlModel->index(sqlModel->rowCount()-1,3).data().toString();
                QString name = sqlModel->index(sqlModel->rowCount()-1,1).data().toString();
                QString surname = sqlModel->index(sqlModel->rowCount()-1,2).data().toString();
                QString msg;
                if(company.isEmpty())
                    msg = name + " " + surname;
                else
                    msg = name + " " + surname + ", " + company;

                trayIcon->showMessage(title, msg, QSystemTrayIcon::Information, 10000);
            }
            else {

                QString title = "Nowa grupa w firmie:";
                trayIcon->showMessage(title, sqlModel->index(sqlModel->rowCount()-1,3).data().toString(), QSystemTrayIcon::Information, 10000);
            }
       }
}

void MainWindow::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    restoreAction->setEnabled(!visible);
    QMainWindow::setVisible(visible);

    if(visible) {
        dcAction = minimizeAction;
        minimizeAction->setFont(QFont("Segou UI", 9, QFont::Bold));
        restoreAction->setFont(QFont("Segou UI", 9));
    }
    else {
        dcAction = restoreAction;
        restoreAction->setFont(QFont("Segou UI", 9, QFont::Bold));
        minimizeAction->setFont(QFont("Segou UI", 9));
    }
}

void MainWindow::loadTrayIcon()
{
    createActions();
    createTrayIcon();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    setIcon();

}

void MainWindow::showTrayIcon()
{
   trayIcon->show();
}
