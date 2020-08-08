/*
 * ChessBoard Class
 *
 * Copyright (C) Kadir Yanık - <kdrynkk@gmail.com>, 2020
 */
#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "chesspiece.h"

#include <QWidget>

/*---------------------------------------------------------------------------*/
#define TOTAL_PIECE_NUM 32

/*---------------------------------------------------------------------------*/
class ChessBoard : public QWidget
{
    Q_OBJECT
public:
    explicit ChessBoard(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent * event);
    void mousePressEvent(QMouseEvent* event);
private:
    void initilizePieces();
    ChessPiece chessPieces[TOTAL_PIECE_NUM];
signals:

};

#endif // CHESSBOARD_H
