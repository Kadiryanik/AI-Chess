/*
 * ChessPiece Class
 *
 * Copyright (C) Kadir Yanık - <kdrynkk@gmail.com>, 2020
 */
#include "chesspiece.h"

/*---------------------------------------------------------------------------*/
ChessPiece::ChessPiece()
{
}

/*---------------------------------------------------------------------------*/
void ChessPiece::setPieceInfo(uint8_t x, uint8_t y, uint8_t type, bool isWhite)
{
    this->xVar = x;
    this->yVar = y;
    this->type = type;
    this->isWhite = isWhite;
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
uint8_t ChessPiece::yInvert()
{
    return 7 - this->yVar;
}

/*---------------------------------------------------------------------------*/
bool ChessPiece::onBoard()
{
    return this->onBoardVar;
}

/*---------------------------------------------------------------------------*/
uint8_t ChessPiece::point(){
    int ret = -1;
    if(isWhite){
        ret = 1;
    }

    switch(this->type){
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
QImage ChessPiece::getImage()
{
    if(this->isWhite){
        switch(this->type){
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
        switch(this->type){
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
