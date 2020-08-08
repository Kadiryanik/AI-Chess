/*
 * ChessBoard Class
 *
 * Copyright (C) Kadir Yanık - <kdrynkk@gmail.com>, 2020
 */
#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QWidget>

/*---------------------------------------------------------------------------*/
class ChessBoard : public QWidget
{
    Q_OBJECT
public:
    explicit ChessBoard(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent * event);
    void mousePressEvent(QMouseEvent* event);
signals:

};

#endif // CHESSBOARD_H
