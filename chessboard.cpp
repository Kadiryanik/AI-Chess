/*
 * ChessBoard Class
 *
 * Copyright (C) Kadir Yanık - <kdrynkk@gmail.com>, 2020
 */
#include "chessboard.h"
#include "chesspiece.h"

#include <QPainter>
#include <QMouseEvent>
#include <QCoreApplication>

/*---------------------------------------------------------------------------*/
#define CB_EACH_BOX_SIZE           64
#define CB_SIZE                    (BOARD_MATRIX_SIZE * CB_EACH_BOX_SIZE)
#define BOX_OFFSET_FOR_IMAGE       2 // for centering image in box
#define POSSIBLE_MOVEMENT_CIRCLE_R 14

#define CB_BG_COLOR_1     (QColor(255, 178, 102))
#define CB_BG_COLOR_2     (QColor(255, 128, 0))
#define CB_SELECTED_COLOR (QColor(255, 10, 10, 180))
#define CB_POSSIBLE_COLOR (QColor(255, 10, 10, 200))
#define CB_HIT_COLOR      (QColor(255, 10, 10, 135))

/*---------------------------------------------------------------------------*/
#define MOVEMENT_NUM   8
#define POSITION_DEPTH 2

const int8_t knightMovementOffsets[MOVEMENT_NUM][POSITION_DEPTH] = {
  { -1, -2 },
  { -2, -1 },
  { -2, 1 },
  { -1, 2 },
  { 1, 2 },
  { 2, 1 },
  { 2, -1 },
  { 1, -2 }
};

const int8_t kingMovementOffsets[MOVEMENT_NUM][POSITION_DEPTH] = {
  { -1, -1 },
  { -1, 0 },
  { -1, 1 },
  { 0, -1 },
  { 0, 1 },
  { 1, -1 },
  { 1, 0 },
  { 1, 1 }
};

/*---------------------------------------------------------------------------*/
ChessBoard::ChessBoard(QWidget *parent) : QWidget(parent)
{
    for(uint8_t i = 0; i < BOARD_MATRIX_SIZE; i++){
        for(uint8_t j = 0; j < BOARD_MATRIX_SIZE; j++){
            boardMatrix[i][j] = -1;
        }
    }

    initilizePieces();

    selectedIndex = -1;
    possibleMoveCount = 0;

    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);

    resize(CB_SIZE, CB_SIZE);
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
    for(int i = 0; i < CB_SIZE; i += CB_EACH_BOX_SIZE){
        for(int j = 0; j < CB_SIZE; j += CB_EACH_BOX_SIZE){
            if(((i + j) / CB_EACH_BOX_SIZE) % 2 == 0){
                painter.fillRect(i, j, CB_EACH_BOX_SIZE, CB_EACH_BOX_SIZE, \
                                 QBrush(CB_BG_COLOR_1));
            } else{
                painter.fillRect(i, j, CB_EACH_BOX_SIZE, CB_EACH_BOX_SIZE, \
                                 QBrush(CB_BG_COLOR_2));
            }
        }
    }

    // paint possible moves if exist
    if(selectedIndex >= 0){
        painter.fillRect(chessPieces[selectedIndex].x() * CB_EACH_BOX_SIZE, \
                    (INVERTING_OFFSET - chessPieces[selectedIndex].y()) * \
                    CB_EACH_BOX_SIZE, CB_EACH_BOX_SIZE, CB_EACH_BOX_SIZE, \
                    QBrush(CB_SELECTED_COLOR));

        for(uint8_t i = 0; i < possibleMoveCount; i++){
            painter.setBrush(QBrush(CB_POSSIBLE_COLOR));

            if(boardMatrix[possibleMoves[i].x][possibleMoves[i].y] == -1){
                painter.drawEllipse(CB_EACH_BOX_SIZE / 2 - \
                    (POSSIBLE_MOVEMENT_CIRCLE_R / 2) + possibleMoves[i].x * \
                    CB_EACH_BOX_SIZE, CB_EACH_BOX_SIZE / 2 - \
                    (POSSIBLE_MOVEMENT_CIRCLE_R / 2) + \
                    (INVERTING_OFFSET - possibleMoves[i].y) * \
                    CB_EACH_BOX_SIZE, POSSIBLE_MOVEMENT_CIRCLE_R, \
                    POSSIBLE_MOVEMENT_CIRCLE_R);
            } else{
                painter.fillRect(possibleMoves[i].x * CB_EACH_BOX_SIZE, \
                    (INVERTING_OFFSET - possibleMoves[i].y) * \
                    CB_EACH_BOX_SIZE, CB_EACH_BOX_SIZE, CB_EACH_BOX_SIZE, \
                    QBrush(CB_HIT_COLOR));
            }
        }
    }

    // draw pieces
    for(int i = 0; i < TOTAL_PIECE_NUM; i++){
        if(chessPieces[i].onBoard()){
            int x = BOX_OFFSET_FOR_IMAGE + \
                    (chessPieces[i].x() * CB_EACH_BOX_SIZE);
            int y = BOX_OFFSET_FOR_IMAGE + ((INVERTING_OFFSET - \
                    chessPieces[i].y()) * CB_EACH_BOX_SIZE);
            painter.drawImage(x, y, chessPieces[i].getImage());
        }
    }

    // TODO: write texts 1..8 & A..H
    painter.restore();
}

/*---------------------------------------------------------------------------*/
void ChessBoard::mousePressEvent(QMouseEvent *event)
{
    uint8_t x = event->pos().x() / CB_EACH_BOX_SIZE;
    // invert y to get correct index
    uint8_t y = INVERTING_OFFSET - (event->pos().y() / CB_EACH_BOX_SIZE);

    if(selectedIndex < 0){
        selectedIndex = boardMatrix[x][y];
        if(selectedIndex < 0){
            return; // index not valid
        } /*else if(!chessPieces[selectedIndex].isWhite()){
            selectedIndex = -1;
            return; // black piece can't select
        }*/

        fillPossibleMoves();
    } else{
        for(uint8_t i = 0; i < possibleMoveCount; i++){
            if(possibleMoves[i].x == x && possibleMoves[i].y == y){
                // clear if any piece exist in next box
                int8_t nextIndex = boardMatrix[x][y];
                if(nextIndex >= 0){
                    chessPieces[nextIndex].remove(); // clear onBoard flag
                }

                uint8_t oldX = chessPieces[selectedIndex].x();
                uint8_t oldY = chessPieces[selectedIndex].y();

                // set new position
                chessPieces[selectedIndex].setPosition(x, y);
                boardMatrix[x][y] = selectedIndex;

                // set old position as not in use
                boardMatrix[oldX][oldY] = -1;

                // TODO: if piece is pawn and reached end of the board
                // change piece.

                break;
            }
        }
        possibleMoveCount = 0;
        selectedIndex = -1;
    }

    this->repaint();
}

/*---------------------------------------------------------------------------*/
void ChessBoard::initilizePieces()
{
    uint8_t x = 0, y = INVERTING_OFFSET, offset = 0;

    // black other pieces
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_ROOK, false);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KNIGHT, false);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_BISHOP, false);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_QUEEN, false);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KING, false);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_BISHOP, false);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KNIGHT, false);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_ROOK, false);

    // black pawns
    x = 0;
    y--;
    for(int i = 0; i < BOARD_MATRIX_SIZE; i++){
        boardMatrix[x][y] = offset;
        chessPieces[offset++].setPieceInfo(x++, y, PIECE_PAWN, false);
    }

    // white pawns
    x = 0;
    y = 1;
    for(int i = 0; i < BOARD_MATRIX_SIZE; i++){
        boardMatrix[x][y] = offset;
        chessPieces[offset++].setPieceInfo(x++, y, PIECE_PAWN, true);
    }

    // white other pieces
    x = 0;
    y--;

    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_ROOK, true);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KNIGHT, true);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_BISHOP, true);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_QUEEN, true);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KING, true);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_BISHOP, true);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KNIGHT, true);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_ROOK, true);
}

/*---------------------------------------------------------------------------*/
void ChessBoard::fillPossibleMoves()
{
    possibleMoveCount = 0;
    switch (chessPieces[selectedIndex].type()){
        case PIECE_BISHOP:
            fillCrossMoves();
            break;
        case PIECE_KING:
        {
            int8_t x = chessPieces[selectedIndex].x();
            int8_t y = chessPieces[selectedIndex].y();

            for(uint8_t i = 0; i < MOVEMENT_NUM; i++){
                int8_t newX = x + kingMovementOffsets[i][0];
                int8_t newY = y + kingMovementOffsets[i][1];

                if(newX >= 0 && newX < BOARD_MATRIX_SIZE && \
                        newY >= 0 && newY < BOARD_MATRIX_SIZE){
                    int8_t index = boardMatrix[newX][newY];

                    if(index == -1 || chessPieces[index].isWhite() != \
                                     chessPieces[selectedIndex].isWhite()){
                        possibleMoves[possibleMoveCount].x = newX;
                        possibleMoves[possibleMoveCount++].y = newY;
                    }
                }
            }

            // TODO: check next boxes make sure other king not there!
        }
            break;
        case PIECE_KNIGHT:
        {
            int8_t x = chessPieces[selectedIndex].x();
            int8_t y = chessPieces[selectedIndex].y();

            for(uint8_t i = 0; i < MOVEMENT_NUM; i++){
                int8_t newX = x + knightMovementOffsets[i][0];
                int8_t newY = y + knightMovementOffsets[i][1];

                if(newX >= 0 && newX < BOARD_MATRIX_SIZE && \
                        newY >= 0 && newY < BOARD_MATRIX_SIZE){
                    int8_t index = boardMatrix[newX][newY];

                    if(index == -1 || chessPieces[index].isWhite() != \
                                     chessPieces[selectedIndex].isWhite()){
                        possibleMoves[possibleMoveCount].x = newX;
                        possibleMoves[possibleMoveCount++].y = newY;
                    }
                }
            }
        }
            break;
        case PIECE_PAWN:
        {
            int8_t x = chessPieces[selectedIndex].x();
            int8_t y = chessPieces[selectedIndex].y();

            int8_t direction = -1;
            if(chessPieces[selectedIndex].isWhite()){
                direction = 1;
            }

            // 1 box forward
            int8_t newX = x;
            int8_t newY = y + direction;
            if(newY >= 0 && newY < BOARD_MATRIX_SIZE && \
                    boardMatrix[newX][newY] == -1){
                possibleMoves[possibleMoveCount].x = newX;
                possibleMoves[possibleMoveCount++].y = newY;

                // if reachs here mean 1 box available check 2 box forward too
                newX = x;
                newY = y + (2 * direction);
                if(!chessPieces[selectedIndex].isMoved() && \
                        newY >= 0 && newY < BOARD_MATRIX_SIZE \
                        && boardMatrix[newX][newY] == -1){
                    possibleMoves[possibleMoveCount].x = newX;
                    possibleMoves[possibleMoveCount++].y = newY;

                    // TODO: mark as 2 box forwarded for be edible
                }
            }

            // w-left & b-right eat
            newX = x - 1;
            newY = y + direction;
            if(newY >= 0 && newY < BOARD_MATRIX_SIZE && newX >= 0 && \
                    boardMatrix[newX][newY] != -1 && \
                    chessPieces[boardMatrix[newX][newY]].isWhite() != \
                    chessPieces[selectedIndex].isWhite()){
                possibleMoves[possibleMoveCount].x = newX;
                possibleMoves[possibleMoveCount++].y = newY;
            }

            // w-right & b-left eat
            newX = x + 1;
            if(newY >= 0 && newY < BOARD_MATRIX_SIZE && \
                    newX < BOARD_MATRIX_SIZE && \
                    boardMatrix[newX][newY] != -1 && \
                    chessPieces[boardMatrix[newX][newY]].isWhite() != \
                    chessPieces[selectedIndex].isWhite()){
                possibleMoves[possibleMoveCount].x = newX;
                possibleMoves[possibleMoveCount++].y = newY;
            }
        }
            break;
        case PIECE_QUEEN:
            fillCrossMoves();
            fillStraightMoves();
            break;
        case PIECE_ROOK:
            fillStraightMoves();
            break;
    }
}

/*---------------------------------------------------------------------------*/
void ChessBoard::fillCrossMoves(){
    int8_t x = chessPieces[selectedIndex].x();
    int8_t y = chessPieces[selectedIndex].y();

    // left up
    for(int8_t i = x - 1, j = y + 1; \
        i >= 0 && j < BOARD_MATRIX_SIZE; i--, j++){
        if(boardMatrix[i][j] != -1){
            if(chessPieces[boardMatrix[i][j]].isWhite() != \
                    chessPieces[selectedIndex].isWhite()){
                possibleMoves[possibleMoveCount].x = i;
                possibleMoves[possibleMoveCount++].y = j;
            }
            break;
        }

        possibleMoves[possibleMoveCount].x = i;
        possibleMoves[possibleMoveCount++].y = j;
    }

    // left down
    for(int8_t i = x - 1, j = y - 1; i >= 0 && j >= 0; i--, j--){
        if(boardMatrix[i][j] != -1){
            if(chessPieces[boardMatrix[i][j]].isWhite() != \
                    chessPieces[selectedIndex].isWhite()){
                possibleMoves[possibleMoveCount].x = i;
                possibleMoves[possibleMoveCount++].y = j;
            }
            break;
        }

        possibleMoves[possibleMoveCount].x = i;
        possibleMoves[possibleMoveCount++].y = j;
    }

    // right up
    for(int8_t i = x + 1, j = y + 1; \
        i < BOARD_MATRIX_SIZE && j < BOARD_MATRIX_SIZE; i++, j++){
        if(boardMatrix[i][j] != -1){
            if(chessPieces[boardMatrix[i][j]].isWhite() != \
                    chessPieces[selectedIndex].isWhite()){
                possibleMoves[possibleMoveCount].x = i;
                possibleMoves[possibleMoveCount++].y = j;
            }
            break;
        }

        possibleMoves[possibleMoveCount].x = i;
        possibleMoves[possibleMoveCount++].y = j;
    }

    // right down
    for(int8_t i = x + 1, j = y - 1; \
        i < BOARD_MATRIX_SIZE && j >= 0; i++, j--){
        if(boardMatrix[i][j] != -1){
            if(chessPieces[boardMatrix[i][j]].isWhite() != \
                    chessPieces[selectedIndex].isWhite()){
                possibleMoves[possibleMoveCount].x = i;
                possibleMoves[possibleMoveCount++].y = j;
            }
            break;
        }

        possibleMoves[possibleMoveCount].x = i;
        possibleMoves[possibleMoveCount++].y = j;
    }
}

/*---------------------------------------------------------------------------*/
void ChessBoard::fillStraightMoves(){
    int8_t x = chessPieces[selectedIndex].x();
    int8_t y = chessPieces[selectedIndex].y();

    // up
    for(int8_t i = x - 1; i >= 0; i--){
        if(boardMatrix[i][y] != -1){
            if(chessPieces[boardMatrix[i][y]].isWhite() != \
                    chessPieces[selectedIndex].isWhite()){
                possibleMoves[possibleMoveCount].x = i;
                possibleMoves[possibleMoveCount++].y = y;
            }
            break;
        }

        possibleMoves[possibleMoveCount].x = i;
        possibleMoves[possibleMoveCount++].y = y;
    }

    // down
    for(int8_t i = x + 1; i < BOARD_MATRIX_SIZE; i++){
        if(boardMatrix[i][y] != -1){
            if(chessPieces[boardMatrix[i][y]].isWhite() != \
                    chessPieces[selectedIndex].isWhite()){
                possibleMoves[possibleMoveCount].x = i;
                possibleMoves[possibleMoveCount++].y = y;
            }
            break;
        }

        possibleMoves[possibleMoveCount].x = i;
        possibleMoves[possibleMoveCount++].y = y;
    }

    // left
    for(int8_t j = y - 1; j >= 0; j--){
        if(boardMatrix[x][j] != -1){
            if(chessPieces[boardMatrix[x][j]].isWhite() != \
                    chessPieces[selectedIndex].isWhite()){
                possibleMoves[possibleMoveCount].x = x;
                possibleMoves[possibleMoveCount++].y = j;
            }
            break;
        }

        possibleMoves[possibleMoveCount].x = x;
        possibleMoves[possibleMoveCount++].y = j;
    }

    // right
    for(int8_t j = y + 1; j < BOARD_MATRIX_SIZE; j++){
        if(boardMatrix[x][j] != -1){
            if(chessPieces[boardMatrix[x][j]].isWhite() != \
                    chessPieces[selectedIndex].isWhite()){
                possibleMoves[possibleMoveCount].x = x;
                possibleMoves[possibleMoveCount++].y = j;
            }
            break;
        }

        possibleMoves[possibleMoveCount].x = x;
        possibleMoves[possibleMoveCount++].y = j;
    }
}
