#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QResizeEvent>
#include <QEvent>

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

private slots:
    void on_sendAccess(QString login,QString password);
    void on_mainButtonReleased(const QPushButton * mainButton);

signals:
    void mainButtonReleased(const QPushButton * mainButton);

private:
    Ui::MainWindow *ui;
    bool readOnly;
    QLabel * Statlabel;
    QProgressBar *Statprogress;

    bool eventFilter(QObject *obj, QEvent *event);
    void resizeEvent(QResizeEvent* event);

    void addStatusBar();
};

#endif // MAINWINDOW_H
