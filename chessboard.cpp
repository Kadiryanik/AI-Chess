/*
 * ChessBoard Class
 *
 * Copyright (C) Kadir Yanık - <kdrynkk@gmail.com>, 2020
 */
#include "chessboard.h"

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

/*---------------------------------------------------------------------------*/
#define CHESS_BOARD_EACH_BOX_SIZE 64
#define CHESS_BOARD_SIZE          (8 * CHESS_BOARD_EACH_BOX_SIZE)

#define CHESS_BOARD_BG_COLOR_1 (QColor(255, 128, 0))
#define CHESS_BOARD_BG_COLOR_2 (QColor(255, 178, 102))

/*---------------------------------------------------------------------------*/
ChessBoard::ChessBoard(QWidget *parent) : QWidget(parent)
{
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

    painter.restore();
}

/*---------------------------------------------------------------------------*/
void ChessBoard::mousePressEvent(QMouseEvent *event)
{
}
