#include "mainwindow.h"
#include <QApplication>
#include <logindialog.h>

int main(int argc, char *argv[])
{
    int currentExitCode = 0;

    do {
        QApplication a(argc, argv);
        LoginDialog *loginDialog = new LoginDialog;
        MainWindow w;

        loginDialog->setWindowFlags(((loginDialog->windowFlags() | Qt::CustomizeWindowHint) & Qt::WindowCloseButtonHint & ~Qt::WindowContextHelpButtonHint));

        if (loginDialog->exec() != QDialog::Accepted) {
            a.quit();
            return 0;
        }
        else {
            delete loginDialog;
            w.showMaximized();
            w.show();
        }
        currentExitCode = a.exec();
    } while( currentExitCode == MainWindow::EXIT_CODE_REBOOT );

    return currentExitCode;
}
