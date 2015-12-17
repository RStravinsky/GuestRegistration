#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    QTimer::singleShot(0,ui->userLineEdit,SLOT(setFocus()));
    connect(ui->userLineEdit,SIGNAL(editingFinished()),ui->passwordLineEdit,SLOT(setFocus()));
    connect(ui->cancelButton,SIGNAL(clicked()),this,SLOT(reject()));

    ui->passwordLineEdit->installEventFilter(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

bool LoginDialog::connectToDatabase(QString &login,QString &password)
{
    sqlDatabase = QSqlDatabase::addDatabase("QMYSQL");
    sqlDatabase.setHostName("192.168.1.7");
    sqlDatabase.setDatabaseName("guestregistration");
    if(login.isEmpty() && password.isEmpty()) {
        sqlDatabase.setUserName("sigmasa");
        sqlDatabase.setPassword("sigmasa");
    }
    else {
        sqlDatabase.setUserName(login);
        sqlDatabase.setPassword(password);
    }

    if (!sqlDatabase.open()) return false;
    else return true;
}

void LoginDialog::closeDatabase(){
    sqlDatabase.close();
    QSqlDatabase::removeDatabase("guestregistration");
}

void LoginDialog::on_loginButton_clicked()
{
    QString username=ui->userLineEdit->text();
    QString password=ui->passwordLineEdit->text();

    if (!connectToDatabase(username,password)) {
        QMessageBox::information(this,"Informacja", "Nie uzyskano dostępu.");
        ui->userLineEdit->clear();
        ui->passwordLineEdit->clear();
        ui->userLineEdit->setFocus();
    }

    else {
        QMessageBox::information(this,"Informacja","Uzyskano dostęp.");
        emit sendAccess(username,password);
        LoginDialog::accept();
    }
}

bool LoginDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(obj == (QObject*)ui->passwordLineEdit)
           {
               if(keyEvent->key() == Qt::Key_Return)
                    on_loginButton_clicked();
           }
    }
    return false;
}


void LoginDialog::on_pushButton_clicked()
{
    static bool isPasswordVisible = true;
    isPasswordVisible = !isPasswordVisible;

    isPasswordVisible == true ? ui->passwordLineEdit->setEchoMode(QLineEdit::Normal)
                              : ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
}
