/*
 * ChessBoard Class
 *
 * Copyright (C) Kadir Yanık - <kdrynkk@gmail.com>, 2020
 */
#include "chessboard.h"
#include "chesspiece.h"

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QCoreApplication>

/*---------------------------------------------------------------------------*/
#define CHESS_BOARD_EACH_BOX_SIZE 64
#define CHESS_BOARD_SIZE          (8 * CHESS_BOARD_EACH_BOX_SIZE)

#define CHESS_BOARD_BG_COLOR_1 (QColor(255, 128, 0))
#define CHESS_BOARD_BG_COLOR_2 (QColor(255, 178, 102))

/*---------------------------------------------------------------------------*/
ChessBoard::ChessBoard(QWidget *parent) : QWidget(parent)
{
    initilizePieces();

    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);

    resize(CHESS_BOARD_SIZE, CHESS_BOARD_SIZE);
    show();
}

/*---------------------------------------------------------------------------*/
void ChessBoard::paintEvent(QPaintEvent *event)
{
    (void)event;
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.save();

    // draw board
    for(int i = 0; i < CHESS_BOARD_SIZE; i += CHESS_BOARD_EACH_BOX_SIZE){
        for(int j = 0; j < CHESS_BOARD_SIZE; j += CHESS_BOARD_EACH_BOX_SIZE){
            if(((i + j) / CHESS_BOARD_EACH_BOX_SIZE) % 2 == 0){
                painter.fillRect(i, j, i + CHESS_BOARD_EACH_BOX_SIZE, \
                                 j + CHESS_BOARD_EACH_BOX_SIZE, \
                                 QBrush(CHESS_BOARD_BG_COLOR_1));
            } else{
                painter.fillRect(i, j, i + CHESS_BOARD_EACH_BOX_SIZE, \
                                 j + CHESS_BOARD_EACH_BOX_SIZE, \
                                 QBrush(CHESS_BOARD_BG_COLOR_2));
            }
        }
    }

    // draw pieces
    int offsetX = 2, offsetY = 2; // for centering image in box
    for(int i = 0; i < TOTAL_PIECE_NUM; i++){
        if(chessPieces[i].onBoard()){

            int x = offsetX + \
                    (chessPieces[i].x() * CHESS_BOARD_EACH_BOX_SIZE);
            int y = offsetY + \
                    (chessPieces[i].yInvert() * CHESS_BOARD_EACH_BOX_SIZE);
            painter.drawImage(x, y, chessPieces[i].getImage());
        }
    }

    painter.restore();
}

/*---------------------------------------------------------------------------*/
void ChessBoard::mousePressEvent(QMouseEvent *event)
{
    (void)event;
}

/*---------------------------------------------------------------------------*/
void ChessBoard::initilizePieces()
{
    int x = 0, y = 7, offset = 0;

    // black other pieces
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_ROOK, false);
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KNIGHT, false);
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_BISHOP, false);
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_QUEEN, false);
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KING, false);
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_BISHOP, false);
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KNIGHT, false);
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_ROOK, false);

    // black pawns
    x = 0;
    y--;
    for(int i = 0; i < 8; i++){
        chessPieces[offset++].setPieceInfo(x++, y, PIECE_PAWN, false);
    }

    // white pawns
    x = 0;
    y = 1;
    for(int i = 0; i < 8; i++){
        chessPieces[offset++].setPieceInfo(x++, y, PIECE_PAWN, true);
    }

    // white other pieces
    x = 0;
    y--;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_ROOK, true);
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KNIGHT, true);
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_BISHOP, true);
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_QUEEN, true);
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KING, true);
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_BISHOP, true);
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KNIGHT, true);
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_ROOK, true);
}
