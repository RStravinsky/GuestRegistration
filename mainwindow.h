#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QResizeEvent>
#include <QEvent>
#include <QKeyEvent>
#include <QSqlTableModel>
#include <QSqlError>
#include <QSortFilterProxyModel>
#include <QSqlQuery>
#include <QTimer>
#include <QtNetwork/QNetworkInterface>
#include <QHeaderView>
#include <QSystemTrayIcon>
#include <qspreadsheetheaderview.h>
#include "logindialog.h"
#include "modsqltablemodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static int const EXIT_CODE_REBOOT=-123456789;
    void loadSqlModel();
    void setVisible(bool visible) Q_DECL_OVERRIDE;

private slots:
    void on_sendAccess(QString login,QString password);
    void on_mainButtonReleased(const QPushButton * mainButton);
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void on_timer_overflow();
    void on_lineEdit_textChanged(const QString &arg1);

    void on_sigmaButton_clicked();

    void setIcon();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

signals:
    void mainButtonReleased(const QPushButton * mainButton);

private:
    Ui::MainWindow *ui;
    bool isAdded{false};
    QLabel * Statlabel;
    QProgressBar *Statprogress;
    ModSqlTableModel * sqlModel;
    QSortFilterProxyModel * proxyModel;
    QTimer *timer;
    bool isConnectedToNetwork();

    QSystemTrayIcon * trayIcon;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QMenu *trayIconMenu;

    void createActions();
    void createTrayIcon();
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void showMessage();
    void setPopupMessage();

    bool eventFilter(QObject *obj, QEvent *event);
    void resizeEvent(QResizeEvent* event);
    void addStatusBar();
    void configureTable();
    bool submit(ModSqlTableModel *&model);
    bool dataIsCorrect();
};

#endif // MAINWINDOW_H
