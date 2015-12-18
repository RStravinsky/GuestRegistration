#include "mainwindow.h"
#include <QApplication>
#include <logindialog.h>

double dpiPercent;

int main(int argc, char *argv[])
{
    int currentExitCode = 0;

    do {
        QApplication a(argc, argv);

        int dpiX = qApp->desktop()->logicalDpiX();
        dpiPercent = ((100*dpiX)/96);

        LoginDialog *loginDialog = new LoginDialog;
        loginDialog->setWindowFlags(((loginDialog->windowFlags() | Qt::CustomizeWindowHint) & Qt::WindowCloseButtonHint & ~Qt::WindowContextHelpButtonHint));
        MainWindow w;
        QObject::connect(loginDialog, SIGNAL(sendAccess(QString,QString)),&w, SLOT(on_sendAccess(QString,QString)));

        if (loginDialog->exec() != QDialog::Accepted) {
            a.quit();
            return 0;
        }
        else {
            delete loginDialog;
            w.showMaximized();
            w.loadSqlModel();
            w.show();
            w.showTrayIcon();
        }
        currentExitCode = a.exec();
    } while( currentExitCode == MainWindow::EXIT_CODE_REBOOT );

    return currentExitCode;
}
