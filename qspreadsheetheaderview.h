#ifndef QSPREADSHEETHEADERVIEW_H
#define QSPREADSHEETHEADERVIEW_H

#include <QHeaderView>
#include <QMessageBox>
#include <mainwindow.h>

/*!
    \class QSpreadsheetHeaderView
    \brief The QSpreadsheetHeaderView class is a special QHeaderView that mimic Google Spreadsheet header.
    version 1.0
    \sa QHeaderView
*/
class QSpreadsheetHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    QSpreadsheetHeaderView(Qt::Orientation orientation, QWidget * parent = 0);
    static QString filter;
protected:
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;

    QRect sectionRect(int logicalIndex) const;
    QRect buttonMenuRect(int logicalIndex) const;
    void drawMenuButton(QPainter *painter, int logicalIndex, bool enabled) const;

    QRect prevRect(int logicalIndex) const;
    QRect nextRect(int logicalIndex) const;
    void drawPrevButton(QPainter *painter, int logicalIndex) const;
    void drawNextButton(QPainter *painter, int logicalIndex) const;

signals:
    void setSQLFilter();

private:

};

#endif /* QSPREADSHEETHEADERVIEW_H */
