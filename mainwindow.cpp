#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    addStatusBar();
    ui->logoutButton->installEventFilter(this);
    ui->printButton->installEventFilter(this);
    ui->helpButton->installEventFilter(this);
    ui->generateButton->installEventFilter(this);
    connect(this,SIGNAL(mainButtonReleased(const QPushButton*)),this,SLOT(on_mainButtonReleased(const QPushButton*)));
}

MainWindow::~MainWindow()
{
    delete ui;
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
    Statprogress->setStyleSheet("QProgressBar {border: 2px solid rgb(20,20,20); border-radius: 5px;background: rgb(20,20,20);min-height: 20px}"
    "QProgressBar::chunk {background: #088E8C ;border-radius: 4px;}");
    Statprogress->setTextVisible(false);
    ui->statusBar->addWidget(Statlabel,1);
    ui->statusBar->addPermanentWidget(Statprogress,1);
    ui->statusBar->setFont(Status_Font);
    ui->statusBar->setStyleSheet("QStatusBar {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 "
    "#a6a6a6, stop: 0.08 #7f7f7f,stop: 0.39999 #717171, stop: 0.4 #626262,stop: 0.9 #4c4c4c,stop: 1 #333333);min-height: 30px}");
}

