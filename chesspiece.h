/*
 * ChessPiece Class
 *
 * Copyright (C) Kadir Yanık - <kdrynkk@gmail.com>, 2020
 */
#ifndef CHESSPIECE_H
#define CHESSPIECE_H

#include <QImage>

/*---------------------------------------------------------------------------*/
#define PIECE_B_BISHOP "../AI-Chess/images/black-bishop.png"
#define PIECE_B_KING   "../AI-Chess/images/black-king.png"
#define PIECE_B_KNIGHT "../AI-Chess/images/black-knight.png"
#define PIECE_B_PAWN   "../AI-Chess/images/black-pawn.png"
#define PIECE_B_QUEEN  "../AI-Chess/images/black-queen.png"
#define PIECE_B_ROOK   "../AI-Chess/images/black-rook.png"

#define PIECE_W_BISHOP "../AI-Chess/images/white-bishop.png"
#define PIECE_W_KING   "../AI-Chess/images/white-king.png"
#define PIECE_W_KNIGHT "../AI-Chess/images/white-knight.png"
#define PIECE_W_PAWN   "../AI-Chess/images/white-pawn.png"
#define PIECE_W_QUEEN  "../AI-Chess/images/white-queen.png"
#define PIECE_W_ROOK   "../AI-Chess/images/white-rook.png"

/*---------------------------------------------------------------------------*/
#define PIECE_BISHOP 0
#define PIECE_KING   1
#define PIECE_KNIGHT 2
#define PIECE_PAWN   3
#define PIECE_QUEEN  4
#define PIECE_ROOK   5

/*---------------------------------------------------------------------------*/
#define PIECE_POINT_PAWN   1
#define PIECE_POINT_BISHOP 3
#define PIECE_POINT_KNIGHT 3
#define PIECE_POINT_ROOK   5
#define PIECE_POINT_QUEEN  9
#define PIECE_POINT_KING   50

/*---------------------------------------------------------------------------*/
#define SIDE_BLACK false
#define SIDE_WHITE true

/*---------------------------------------------------------------------------*/
class ChessPiece
{
public:
    ChessPiece();
    void setPieceInfo(uint8_t x, uint8_t y, uint8_t type, bool side);
    uint8_t x();
    uint8_t y();
    bool isMoved();
    bool onBoard();
    bool side();
    int8_t point();
    uint8_t type();
    void setOnBoard(bool flag);
    void setPosition(uint8_t x, uint8_t y);
    QImage getImage();
private:
    uint8_t xVar; // 0 .. 7 -> A .. H
    uint8_t yVar; // 0 .. 7 -> 1 .. 8
    uint8_t typeVar;
    bool isMovedVar;
    bool sideVar;
    bool onBoardVar;
};

#endif // CHESSPIECE_H
