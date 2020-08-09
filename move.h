/*
 * ChessPiece Move Class
 *
 * Copyright (C) Kadir Yanık - <kdrynkk@gmail.com>, 2020
 */
#ifndef MOVE_H
#define MOVE_H

#include "chesspiece.h"

#include <iostream>

/*---------------------------------------------------------------------------*/
typedef struct{
    uint8_t x;
    uint8_t y;
} pos_t;

/*---------------------------------------------------------------------------*/
class Move
{
public:
    Move();
    Move(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, \
         ChessPiece piece, int8_t index = -1);
    void setPositions(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
    void setMovedPiece(ChessPiece piece);
    void setEatenPieceIndex(int8_t index);
    pos_t from;
    pos_t to;
    bool pieceWasEaten;
    int8_t eatenPieceIndex;
    ChessPiece movedPiece; // for holding prev setting
};

#endif // MOVE_H
