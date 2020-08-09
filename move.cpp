/*
 * ChessPiece Move Class
 *
 * Copyright (C) Kadir Yanık - <kdrynkk@gmail.com>, 2020
 */
#include "move.h"

/*---------------------------------------------------------------------------*/
Move::Move()
{
    this->from.x = 0;
    this->from.y = 0;
    this->to.x = 0;
    this->to.y = 0;
    this->pieceWasEaten = false;
}

/*---------------------------------------------------------------------------*/
Move::Move(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, \
           ChessPiece piece, int8_t index)
{
    this->from.x = x1;
    this->from.y = y1;
    this->to.x = x2;
    this->to.y = y2;
    this->movedPiece = piece;

    if(index >= 0){
        this->pieceWasEaten = true;
        this->eatenPieceIndex = index;
    } else{
        this->pieceWasEaten = false;
    }
}

/*---------------------------------------------------------------------------*/
void Move::setPositions(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    this->from.x = x1;
    this->from.y = y1;
    this->to.x = x2;
    this->to.y = y2;
}

/*---------------------------------------------------------------------------*/
void Move::setMovedPiece(ChessPiece piece)
{
    this->movedPiece = piece;
}

/*---------------------------------------------------------------------------*/
void Move::setEatenPieceIndex(int8_t index)
{
    this->eatenPieceIndex = index;
    if(index >= 0){
        this->pieceWasEaten = true;
    }
}
