/*
 * ChessBoard Class
 *
 * Copyright (C) Kadir Yanık - <kdrynkk@gmail.com>, 2020
 */
#include "chessboard.h"
#include "chesspiece.h"

// for template defination linkage
#include "stack.cpp"

#include <QPainter>
#include <QMouseEvent>
#include <QCoreApplication>

/*---------------------------------------------------------------------------*/
#define CB_EACH_BOX_SIZE           64
#define CB_SIZE                    (BOARD_MATRIX_SIZE * CB_EACH_BOX_SIZE)
#define BOX_OFFSET_FOR_IMAGE       2 // for centering image in box
#define POSSIBLE_MOVEMENT_CIRCLE_R 14

#define AI_SEARCH_DEPTH 5

/* https://chess.stackexchange.com/questions/4113/longest-chess-game-possible
 * -maximum-moves */
#define MAX_MOVES_IN_A_GAME    6400

// 103: P-(8*4) + R-14 + K-8 + B-14 + Q-27 + K-8
#define MAX_MOVES_EACH_TURN    103

#define CB_BG_COLOR_1      (QColor(255, 178, 102))
#define CB_BG_COLOR_2      (QColor(255, 128, 0))
#define CB_SELECTED_COLOR  (QColor(51, 255, 51, 170))
#define CB_POSSIBLE_COLOR  (QColor(51, 255, 51, 150))
#define CB_HIT_COLOR       (QColor(51, 255, 51, 135))
#define CB_LAST_MOVE_COLOR (QColor(255, 30, 30, 135))

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

    movePool = new Stack<Move>(MAX_MOVES_IN_A_GAME);
    selectedIndex = -1;
    legalMoveCount = 0;
    movementSide = SIDE_WHITE;

    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);

    resize(CB_SIZE, CB_SIZE);
    show();
}
/*---------------------------------------------------------------------------*/
ChessBoard::~ChessBoard()
{
    if(movePool != nullptr){
        delete movePool;
    }
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

    Move lastMove;
    if(movePool->peek(&lastMove)){
        painter.fillRect(lastMove.from.x * CB_EACH_BOX_SIZE, \
            (INVERTING_OFFSET - lastMove.from.y) * \
            CB_EACH_BOX_SIZE, CB_EACH_BOX_SIZE, CB_EACH_BOX_SIZE, \
            QBrush(CB_LAST_MOVE_COLOR));
    }

    // paint possible moves if exist
    if(selectedIndex >= 0){
        painter.fillRect(chessPieces[selectedIndex].x() * CB_EACH_BOX_SIZE, \
                    (INVERTING_OFFSET - chessPieces[selectedIndex].y()) * \
                    CB_EACH_BOX_SIZE, CB_EACH_BOX_SIZE, CB_EACH_BOX_SIZE, \
                    QBrush(CB_SELECTED_COLOR));

        for(uint8_t i = 0; i < legalMoveCount; i++){
            painter.setBrush(QBrush(CB_POSSIBLE_COLOR));

            if(boardMatrix[legalMoves[i].to.x][legalMoves[i].to.y] == -1){
                painter.drawEllipse(CB_EACH_BOX_SIZE / 2 - \
                    (POSSIBLE_MOVEMENT_CIRCLE_R / 2) + legalMoves[i].to.x * \
                    CB_EACH_BOX_SIZE, CB_EACH_BOX_SIZE / 2 - \
                    (POSSIBLE_MOVEMENT_CIRCLE_R / 2) + \
                    (INVERTING_OFFSET - legalMoves[i].to.y) * \
                    CB_EACH_BOX_SIZE, POSSIBLE_MOVEMENT_CIRCLE_R, \
                    POSSIBLE_MOVEMENT_CIRCLE_R);
            } else{
                painter.fillRect(legalMoves[i].to.x * CB_EACH_BOX_SIZE, \
                    (INVERTING_OFFSET - legalMoves[i].to.y) * \
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
        } else if(chessPieces[selectedIndex].side() != movementSide){
            selectedIndex = -1;
            return; // not your turn
        }

        legalMoveCount = prepareLegalMoves(legalMoves);
    } else{
        for(uint8_t i = 0; i < legalMoveCount; i++){
            if(legalMoves[i].to.x == x && legalMoves[i].to.y == y){
                makeMove(legalMoves[i]);
                break;
            }
        }
        legalMoveCount = 0;
        selectedIndex = -1;
    }

    this->repaint();

    // TODO: will change this when player can be SIDE_BLACK.
    if(movementSide == SIDE_BLACK){
        makeAIMove();
    }
}

/*---------------------------------------------------------------------------*/
void ChessBoard::initilizePieces()
{
    uint8_t x = 0, y = INVERTING_OFFSET, offset = 0;

    // black other pieces
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_ROOK, SIDE_BLACK);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KNIGHT, SIDE_BLACK);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_BISHOP, SIDE_BLACK);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_QUEEN, SIDE_BLACK);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KING, SIDE_BLACK);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_BISHOP, SIDE_BLACK);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KNIGHT, SIDE_BLACK);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_ROOK, SIDE_BLACK);

    // black pawns
    x = 0;
    y--;
    for(int i = 0; i < BOARD_MATRIX_SIZE; i++){
        boardMatrix[x][y] = offset;
        chessPieces[offset++].setPieceInfo(x++, y, PIECE_PAWN, SIDE_BLACK);
    }

    // white pawns
    x = 0;
    y = 1;
    for(int i = 0; i < BOARD_MATRIX_SIZE; i++){
        boardMatrix[x][y] = offset;
        chessPieces[offset++].setPieceInfo(x++, y, PIECE_PAWN, SIDE_WHITE);
    }

    // white other pieces
    x = 0;
    y--;

    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_ROOK, SIDE_WHITE);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KNIGHT, SIDE_WHITE);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_BISHOP, SIDE_WHITE);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_QUEEN, SIDE_WHITE);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KING, SIDE_WHITE);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_BISHOP, SIDE_WHITE);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KNIGHT, SIDE_WHITE);
    boardMatrix[x][y] = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_ROOK, SIDE_WHITE);
}

/*---------------------------------------------------------------------------*/
uint8_t ChessBoard::getAllMoves(Move *moves)
{
    uint8_t halfNum = TOTAL_PIECE_NUM / 2;
    // black pieces in first half
    uint8_t startIndex = 0, endIndex = halfNum;
    uint8_t moveCount = 0;

    // white pieces in second half
    if(movementSide == SIDE_WHITE){
        startIndex += halfNum;
        endIndex += halfNum;
    }

    // prepare legal moves for each piece
    for(uint8_t i = startIndex; i < endIndex; i++){
        if(chessPieces[i].onBoard()){
            selectedIndex = i;
            moveCount += prepareLegalMoves(&moves[moveCount]);
        }
    }

    // reset the selected index
    selectedIndex = -1;

    return moveCount;
}

/*---------------------------------------------------------------------------*/
uint8_t ChessBoard::prepareLegalMoves(Move *moves)
{
    uint8_t moveCount = 0;
    switch (chessPieces[selectedIndex].type()){
        case PIECE_BISHOP:
            moveCount = fillCrossMoves(moves);
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

                    if(index == -1 || chessPieces[index].side() != \
                                     chessPieces[selectedIndex].side()){
                        moves[moveCount++].setPositions(x, y, newX, newY);
                    }
                }
            }

            // TODO: check next boxes make sure other king not there!
            // Add castling movement.
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

                    if(index == -1 || chessPieces[index].side() != \
                                     chessPieces[selectedIndex].side()){
                        moves[moveCount++].setPositions(x, y, newX, newY);
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
            if(chessPieces[selectedIndex].side() == SIDE_WHITE){
                direction = 1;
            }

            // 1 box forward
            int8_t newX = x;
            int8_t newY = y + direction;
            if(newY >= 0 && newY < BOARD_MATRIX_SIZE && \
                    boardMatrix[newX][newY] == -1){
                moves[moveCount++].setPositions(x, y, newX, newY);

                // if reachs here mean 1 box available check 2 box forward too
                newX = x;
                newY = y + (2 * direction);
                if(!chessPieces[selectedIndex].isMoved() && \
                        newY >= 0 && newY < BOARD_MATRIX_SIZE \
                        && boardMatrix[newX][newY] == -1){
                    moves[moveCount++].setPositions(x, y, newX, newY);

                    // TODO: mark as 2 box forwarded for be edible
                }
            }

            // w-left & b-right eat
            newX = x - 1;
            newY = y + direction;
            if(newY >= 0 && newY < BOARD_MATRIX_SIZE && newX >= 0 && \
                    boardMatrix[newX][newY] != -1 && \
                    chessPieces[boardMatrix[newX][newY]].side() != \
                    chessPieces[selectedIndex].side()){
                moves[moveCount++].setPositions(x, y, newX, newY);
            }

            // w-right & b-left eat
            newX = x + 1;
            if(newY >= 0 && newY < BOARD_MATRIX_SIZE && \
                    newX < BOARD_MATRIX_SIZE && \
                    boardMatrix[newX][newY] != -1 && \
                    chessPieces[boardMatrix[newX][newY]].side() != \
                    chessPieces[selectedIndex].side()){
                moves[moveCount++].setPositions(x, y, newX, newY);
            }
        }
            break;
        case PIECE_QUEEN:
            moveCount = fillCrossMoves(moves);
            moveCount += fillStraightMoves(&moves[moveCount]);
            break;
        case PIECE_ROOK:
            moveCount = fillStraightMoves(moves);
            break;
    }
    return moveCount;
}

/*---------------------------------------------------------------------------*/
uint8_t ChessBoard::fillCrossMoves(Move *moves)
{
    uint8_t moveCount = 0;
    int8_t x = chessPieces[selectedIndex].x();
    int8_t y = chessPieces[selectedIndex].y();

    // left up
    for(int8_t i = x - 1, j = y + 1; \
        i >= 0 && j < BOARD_MATRIX_SIZE; i--, j++){
        if(boardMatrix[i][j] != -1){
            if(chessPieces[boardMatrix[i][j]].side() != \
                    chessPieces[selectedIndex].side()){
                moves[moveCount++].setPositions(x, y, i, j);
            }
            break;
        }

        moves[moveCount++].setPositions(x, y, i, j);
    }

    // left down
    for(int8_t i = x - 1, j = y - 1; i >= 0 && j >= 0; i--, j--){
        if(boardMatrix[i][j] != -1){
            if(chessPieces[boardMatrix[i][j]].side() != \
                    chessPieces[selectedIndex].side()){
                moves[moveCount++].setPositions(x, y, i, j);
            }
            break;
        }

        moves[moveCount++].setPositions(x, y, i, j);
    }

    // right up
    for(int8_t i = x + 1, j = y + 1; \
        i < BOARD_MATRIX_SIZE && j < BOARD_MATRIX_SIZE; i++, j++){
        if(boardMatrix[i][j] != -1){
            if(chessPieces[boardMatrix[i][j]].side() != \
                    chessPieces[selectedIndex].side()){
                moves[moveCount++].setPositions(x, y, i, j);
            }
            break;
        }

        moves[moveCount++].setPositions(x, y, i, j);
    }

    // right down
    for(int8_t i = x + 1, j = y - 1; \
        i < BOARD_MATRIX_SIZE && j >= 0; i++, j--){
        if(boardMatrix[i][j] != -1){
            if(chessPieces[boardMatrix[i][j]].side() != \
                    chessPieces[selectedIndex].side()){
                moves[moveCount++].setPositions(x, y, i, j);
            }
            break;
        }

        moves[moveCount++].setPositions(x, y, i, j);
    }

    return moveCount;
}

/*---------------------------------------------------------------------------*/
uint8_t ChessBoard::fillStraightMoves(Move *moves)
{
    uint8_t moveCount = 0;
    int8_t x = chessPieces[selectedIndex].x();
    int8_t y = chessPieces[selectedIndex].y();

    // up
    for(int8_t i = x - 1; i >= 0; i--){
        if(boardMatrix[i][y] != -1){
            if(chessPieces[boardMatrix[i][y]].side() != \
                    chessPieces[selectedIndex].side()){
                moves[moveCount++].setPositions(x, y, i, y);
            }
            break;
        }

        moves[moveCount++].setPositions(x, y, i, y);
    }

    // down
    for(int8_t i = x + 1; i < BOARD_MATRIX_SIZE; i++){
        if(boardMatrix[i][y] != -1){
            if(chessPieces[boardMatrix[i][y]].side() != \
                    chessPieces[selectedIndex].side()){
                moves[moveCount++].setPositions(x, y, i, y);
            }
            break;
        }

        moves[moveCount++].setPositions(x, y, i, y);
    }

    // left
    for(int8_t j = y - 1; j >= 0; j--){
        if(boardMatrix[x][j] != -1){
            if(chessPieces[boardMatrix[x][j]].side() != \
                    chessPieces[selectedIndex].side()){
                moves[moveCount++].setPositions(x, y, x, j);
            }
            break;
        }

        moves[moveCount++].setPositions(x, y, x, j);
    }

    // right
    for(int8_t j = y + 1; j < BOARD_MATRIX_SIZE; j++){
        if(boardMatrix[x][j] != -1){
            if(chessPieces[boardMatrix[x][j]].side() != \
                    chessPieces[selectedIndex].side()){
                moves[moveCount++].setPositions(x, y, x, j);
            }
            break;
        }

        moves[moveCount++].setPositions(x, y, x, j);
    }

    return moveCount;
}

/*---------------------------------------------------------------------------*/
 void ChessBoard::makeMove(Move move)
 {
     // clear if any piece exist in next box
     uint8_t currentIndex = boardMatrix[move.from.x][move.from.y];
     int8_t nextIndex = boardMatrix[move.to.x][move.to.y];
     if(nextIndex >= 0){
         chessPieces[nextIndex].setOnBoard(false);
     }
     move.setMovedPiece(chessPieces[currentIndex]);
     move.setEatenPieceIndex(nextIndex);

     movePool->push(move);

     // set new position
     chessPieces[currentIndex].setPosition(move.to.x, move.to.y);
     boardMatrix[move.to.x][move.to.y] = currentIndex;

     // set old position as not in use
     boardMatrix[move.from.x][move.from.y] = -1;

     // TODO: if piece is pawn and reached end of the board
     // change piece.

     // turn the side
     movementSide = !movementSide;
 }

/*---------------------------------------------------------------------------*/
void ChessBoard::undoLastMove()
{
    Move move;
    if(movePool->pop(&move)){
        int8_t movedPieceIndex = boardMatrix[move.to.x][move.to.y];

        // move back the piece
        chessPieces[movedPieceIndex] = move.movedPiece;
        boardMatrix[move.from.x][move.from.y] = movedPieceIndex;

        // get the eaten piece back if exist
        if(move.pieceWasEaten){
            chessPieces[move.eatenPieceIndex].setOnBoard(true);
            boardMatrix[move.to.x][move.to.y] = move.eatenPieceIndex;
        } else{
            boardMatrix[move.to.x][move.to.y] = -1;
        }

        // turn the side
        movementSide = !movementSide;
    }
}

/*---------------------------------------------------------------------------*/
void ChessBoard::makeAIMove()
{
    minimax(AI_SEARCH_DEPTH, INT_MIN, INT_MAX, true);
    makeMove(bestMove);

    this->repaint();
}

/*---------------------------------------------------------------------------*/
int ChessBoard::getRating()
{
    int sum = 0;
    for(uint8_t i = 0; i < TOTAL_PIECE_NUM; i++){
        if(chessPieces[i].onBoard()){
            sum += chessPieces[i].point();
        }
    }

    // we are changing movementSide when piece moved so if SIDE_WHITE piece
    // moved lastly, movementSide will be SIDE_BLACK.
    return movementSide == SIDE_BLACK ? sum : (-1 * sum);
}

/*---------------------------------------------------------------------------*/
int ChessBoard::minimax(int depth, int alpha, int beta, bool maximizing)
{
    if(depth == 0){ // TODO: check game over status too
        return getRating();
    }

    Move moves[MAX_MOVES_EACH_TURN];
    int moveCount = getAllMoves(moves);

    if(maximizing){
        for(uint8_t i = 0; i < moveCount; i++){
            makeMove(moves[i]);
            int eval = minimax(depth - 1, alpha, beta, !maximizing);
            undoLastMove();

            if(alpha < eval){
                alpha = eval;

                if(depth == AI_SEARCH_DEPTH){
                    bestMove = moves[i];
                }
            }

            if(beta <= alpha) break;
        }
        return alpha;
    } else{
        for(uint8_t i = 0; i < moveCount; i++){
            makeMove(moves[i]);
            int eval = minimax(depth - 1, alpha, beta, !maximizing);
            undoLastMove();

            beta = (beta > eval) ? eval : beta;
            if(beta <= alpha) break;
        }
        return beta;
    }
}
