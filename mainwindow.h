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
#include <QPainter>
#include <QtPrintSupport>
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


private slots:
    void on_sendAccess(QString login,QString password);
    void on_mainButtonReleased(const QPushButton * mainButton);
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void on_timer_overflow();

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

    bool eventFilter(QObject *obj, QEvent *event);
    void resizeEvent(QResizeEvent* event);
    void addStatusBar();
    void configureTable();
    bool submit(ModSqlTableModel *&model);
    void exportToPDF();
    QPixmap grabTable();
};

#endif // MAINWINDOW_H
