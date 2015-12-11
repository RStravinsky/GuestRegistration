#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QMessageBox>
#include <QSql>
#include <QSqlDatabase>
#include <QKeyEvent>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();
    QSqlDatabase sqlDatabase;
    void closeDatabase();

private slots:
    void on_loginButton_clicked();

signals:
    void sendAccess(QString login, QString password);

private:
    Ui::LoginDialog *ui;
    bool eventFilter(QObject *obj, QEvent *event);
    bool connectToDatabase(QString &login,QString &password);
};

#endif // LOGINDIALOG_H
