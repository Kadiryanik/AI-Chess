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
#define TOTAL_PIECE_NUM   32
#define MAX_POSSIBLE_MOVE 27
#define BOARD_MATRIX_SIZE 8 // same row & column
#define INVERTING_OFFSET  (BOARD_MATRIX_SIZE - 1)

/*---------------------------------------------------------------------------*/
typedef struct{
    uint8_t x;
    uint8_t y;
} pos_t;

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
    // holds the indexes
    int8_t boardMatrix[BOARD_MATRIX_SIZE][BOARD_MATRIX_SIZE];
    // piece properties
    ChessPiece chessPieces[TOTAL_PIECE_NUM];

    // for moving pieces
    void fillPossibleMoves();
    void fillStraightMoves();
    void fillCrossMoves();
    int8_t selectedIndex;
    // array used for improving performance
    pos_t possibleMoves[MAX_POSSIBLE_MOVE];
    uint8_t possibleMoveCount;
signals:

};

#endif // CHESSBOARD_H
