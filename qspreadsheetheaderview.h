#ifndef QSPREADSHEETHEADERVIEW_H
#define QSPREADSHEETHEADERVIEW_H

#include <QHeaderView>

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
protected:
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;

    QRect sectionRect(int logicalIndex) const;
    QRect buttonMenuRect(int logicalIndex) const;
    void drawMenuButton(QPainter *painter, int logicalIndex, bool enabled) const;

private:

};

#endif /* QSPREADSHEETHEADERVIEW_H */
