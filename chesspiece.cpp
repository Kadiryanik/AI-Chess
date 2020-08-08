/*
 * ChessPiece Class
 *
 * Copyright (C) Kadir Yanık - <kdrynkk@gmail.com>, 2020
 */
#include "chesspiece.h"

/*---------------------------------------------------------------------------*/
ChessPiece::ChessPiece()
{
    this->isMovedVar = false;
}

/*---------------------------------------------------------------------------*/
void ChessPiece::setPieceInfo(uint8_t x, uint8_t y, uint8_t type, bool isWhite)
{
    this->xVar = x;
    this->yVar = y;
    this->typeVar = type;
    this->isWhiteVar = isWhite;
    this->onBoardVar = true;
}

/*---------------------------------------------------------------------------*/
uint8_t ChessPiece::x()
{
    return this->xVar;
}

/*---------------------------------------------------------------------------*/
uint8_t ChessPiece::y()
{
    return this->yVar;
}

/*---------------------------------------------------------------------------*/
bool ChessPiece::isMoved()
{
    return this->isMovedVar;
}

/*---------------------------------------------------------------------------*/
bool ChessPiece::onBoard()
{
    return this->onBoardVar;
}

/*---------------------------------------------------------------------------*/
bool ChessPiece::isWhite()
{
    return this->isWhiteVar;
}

/*---------------------------------------------------------------------------*/
uint8_t ChessPiece::point(){
    int ret = -1;
    if(isWhiteVar){
        ret = 1;
    }

    switch(this->typeVar){
        case PIECE_BISHOP:
            ret *= PIECE_POINT_BISHOP;
        case PIECE_KING:
            ret *= PIECE_POINT_KING;
        case PIECE_KNIGHT:
            ret *= PIECE_POINT_KNIGHT;
        case PIECE_PAWN:
            ret *= PIECE_POINT_PAWN;
        case PIECE_QUEEN:
            ret *= PIECE_POINT_QUEEN;
        case PIECE_ROOK:
            ret *= PIECE_POINT_ROOK;
    }
    return ret;
}

/*---------------------------------------------------------------------------*/
uint8_t ChessPiece::type(){
    return this->typeVar;
}

/*---------------------------------------------------------------------------*/
void ChessPiece::remove(){
    this->onBoardVar = false;
}

/*---------------------------------------------------------------------------*/
void ChessPiece::setPosition(uint8_t x, uint8_t y){
    this->xVar = x;
    this->yVar = y;

    this->isMovedVar = true;
}

/*---------------------------------------------------------------------------*/
QImage ChessPiece::getImage()
{
    if(this->isWhiteVar){
        switch(this->typeVar){
            case PIECE_BISHOP:
                return QImage(PIECE_W_BISHOP);
            case PIECE_KING:
                return QImage(PIECE_W_KING);
            case PIECE_KNIGHT:
                return QImage(PIECE_W_KNIGHT);
            case PIECE_PAWN:
                return QImage(PIECE_W_PAWN);
            case PIECE_QUEEN:
                return QImage(PIECE_W_QUEEN);
            case PIECE_ROOK:
                return QImage(PIECE_W_ROOK);
        }
    } else{
        switch(this->typeVar){
            case PIECE_BISHOP:
                return QImage(PIECE_B_BISHOP);
            case PIECE_KING:
                return QImage(PIECE_B_KING);
            case PIECE_KNIGHT:
                return QImage(PIECE_B_KNIGHT);
            case PIECE_PAWN:
                return QImage(PIECE_B_PAWN);
            case PIECE_QUEEN:
                return QImage(PIECE_B_QUEEN);
            case PIECE_ROOK:
                return QImage(PIECE_B_ROOK);
        }
    }

    return QImage();
}
