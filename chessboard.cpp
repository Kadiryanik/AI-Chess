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
            boardInfo[i][j].index = -1;
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

            if(boardInfo[legalMoves[i].to.x][legalMoves[i].to.y].index == -1){
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
        selectedIndex = boardInfo[x][y].index;
        if(selectedIndex < 0){
            return; // index not valid
        } else if(chessPieces[selectedIndex].side() != movementSide){
            selectedIndex = -1;
            return; // not your turn
        }

        legalMoveCount = prepareLegalMoves(chessPieces[selectedIndex], \
                                           legalMoves);
    } else{
        for(uint8_t i = 0; i < legalMoveCount; i++){
            if(legalMoves[i].to.x == x && legalMoves[i].to.y == y){
                makeMove(legalMoves[i]);
                updatePressures();
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
    boardInfo[x][y].index = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_ROOK, SIDE_BLACK);
    boardInfo[x][y].index = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KNIGHT, SIDE_BLACK);
    boardInfo[x][y].index = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_BISHOP, SIDE_BLACK);
    boardInfo[x][y].index = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_QUEEN, SIDE_BLACK);
    boardInfo[x][y].index = offset;

    blackKingIndex = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KING, SIDE_BLACK);

    boardInfo[x][y].index = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_BISHOP, SIDE_BLACK);
    boardInfo[x][y].index = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KNIGHT, SIDE_BLACK);
    boardInfo[x][y].index = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_ROOK, SIDE_BLACK);

    // black pawns
    x = 0;
    y--;
    for(int i = 0; i < BOARD_MATRIX_SIZE; i++){
        boardInfo[x][y].index = offset;
        chessPieces[offset++].setPieceInfo(x++, y, PIECE_PAWN, SIDE_BLACK);
    }

    // white pawns
    x = 0;
    y = 1;
    for(int i = 0; i < BOARD_MATRIX_SIZE; i++){
        boardInfo[x][y].index = offset;
        chessPieces[offset++].setPieceInfo(x++, y, PIECE_PAWN, SIDE_WHITE);
    }

    // white other pieces
    x = 0;
    y--;

    boardInfo[x][y].index = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_ROOK, SIDE_WHITE);
    boardInfo[x][y].index = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KNIGHT, SIDE_WHITE);
    boardInfo[x][y].index = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_BISHOP, SIDE_WHITE);
    boardInfo[x][y].index = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_QUEEN, SIDE_WHITE);
    boardInfo[x][y].index = offset;

    whiteKingIndex = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KING, SIDE_WHITE);

    boardInfo[x][y].index = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_BISHOP, SIDE_WHITE);
    boardInfo[x][y].index = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_KNIGHT, SIDE_WHITE);
    boardInfo[x][y].index = offset;
    chessPieces[offset++].setPieceInfo(x++, y, PIECE_ROOK, SIDE_WHITE);
}

/*---------------------------------------------------------------------------*/
void ChessBoard::updatePressures(uint8_t (*pressures)[BOARD_MATRIX_SIZE])
{
    if(pressures == nullptr){
        for(uint8_t i = 0; i < BOARD_MATRIX_SIZE; i++){
            for(uint8_t j = 0; j < BOARD_MATRIX_SIZE; j++){
                boardInfo[i][j].pressure = 0;
            }
        }
    } else{
        for(uint8_t i = 0; i < BOARD_MATRIX_SIZE; i++){
            for(uint8_t j = 0; j < BOARD_MATRIX_SIZE; j++){
                pressures[i][j] = 0;
            }
        }
    }

    Move moves[MAX_MOVES_EACH_TURN];

    // change movement side for getting other side's moving list
    movementSide = !movementSide;
    uint8_t moveCount = getAllMoves(moves, true);
    movementSide = !movementSide; // set back

    if(pressures == nullptr){
        for(uint8_t i = 0; i < moveCount; i++){
                boardInfo[moves[i].to.x][moves[i].to.y].pressure++;
        }
    } else{
        for(uint8_t i = 0; i < moveCount; i++){
                pressures[moves[i].to.x][moves[i].to.y]++;
        }
    }
}

/*---------------------------------------------------------------------------*/
bool ChessBoard::isKingUnderPressure(uint8_t (*pressures)[BOARD_MATRIX_SIZE])
{
    uint8_t kingIndex = whiteKingIndex;
    if(movementSide == SIDE_BLACK){
        kingIndex = blackKingIndex;
    }

    if(pressures == nullptr){
        if(boardInfo[chessPieces[kingIndex].x()]\
                [chessPieces[kingIndex].y()].pressure != 0){
            return true;
        }
    } else{
        if(pressures[chessPieces[kingIndex].x()]\
                [chessPieces[kingIndex].y()] != 0){
            return true;
        }
    }

    return false;
}

/*---------------------------------------------------------------------------*/
bool ChessBoard::checkKingPressure(Move *move)
{
    uint8_t pressures[BOARD_MATRIX_SIZE][BOARD_MATRIX_SIZE];

    makeMove(*move, false);
    updatePressures(pressures); // set pressures after move
    undoLastMove(false);

    // check the move is removing the pressure
    if(isKingUnderPressure(pressures)){
        return false; // still under pressure
    }

    return true; // no pressure
}

/*---------------------------------------------------------------------------*/
uint8_t ChessBoard::getAllMoves(Move *moves, bool pressureChecking)
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
            moveCount += prepareLegalMoves(chessPieces[i], &moves[moveCount], \
                                           pressureChecking);
        }
    }

    return moveCount;
}

/*---------------------------------------------------------------------------*/
uint8_t ChessBoard::prepareLegalMoves(ChessPiece piece, Move *moves, \
                                      bool pressureChecking)
{
    uint8_t moveCount = 0;
    switch (piece.type()){
        case PIECE_BISHOP:
            moveCount = fillCrossMoves(piece, moves, pressureChecking);
            break;
        case PIECE_KING:
        {
            int8_t x = piece.x();
            int8_t y = piece.y();

            for(uint8_t i = 0; i < MOVEMENT_NUM; i++){
                int8_t newX = x + kingMovementOffsets[i][0];
                int8_t newY = y + kingMovementOffsets[i][1];

                if(newX >= 0 && newX < BOARD_MATRIX_SIZE && \
                        newY >= 0 && newY < BOARD_MATRIX_SIZE){
                    int8_t index = boardInfo[newX][newY].index;

                    if(index == -1 || pressureChecking || \
                            chessPieces[index].side() != piece.side()){
                        if(boardInfo[newX][newY].pressure == 0){
                            moves[moveCount++].setPositions(x, y, newX, newY);
                        }
                    }
                }
            }

            // TODO: Add castling movement.
        }
            break;
        case PIECE_KNIGHT:
        {
            int8_t x = piece.x();
            int8_t y = piece.y();

            for(uint8_t i = 0; i < MOVEMENT_NUM; i++){
                int8_t newX = x + knightMovementOffsets[i][0];
                int8_t newY = y + knightMovementOffsets[i][1];

                if(newX >= 0 && newX < BOARD_MATRIX_SIZE && \
                        newY >= 0 && newY < BOARD_MATRIX_SIZE){
                    int8_t index = boardInfo[newX][newY].index;

                    if(index == -1 || pressureChecking || \
                            chessPieces[index].side() != piece.side()){
                        moves[moveCount++].setPositions(x, y, newX, newY);
                    }
                }
            }
        }
            break;
        case PIECE_PAWN:
        {
            int8_t x = piece.x();
            int8_t y = piece.y();

            int8_t direction = -1;
            if(piece.side() == SIDE_WHITE){
                direction = 1;
            }

            // 1 box forward
            int8_t newX = x;
            int8_t newY = y + direction;
            if(newY >= 0 && newY < BOARD_MATRIX_SIZE && \
                    boardInfo[newX][newY].index == -1 && \
                    !pressureChecking){
                moves[moveCount++].setPositions(x, y, newX, newY);

                // if reachs here mean 1 box available check 2 box forward too
                newX = x;
                newY = y + (2 * direction);
                if(!piece.isMoved() && newY >= 0 && newY < BOARD_MATRIX_SIZE \
                        && boardInfo[newX][newY].index == -1 \
                        && !pressureChecking){
                    moves[moveCount++].setPositions(x, y, newX, newY);

                    // TODO: mark as 2 box forwarded for be edible
                }
            }

            // w-left & b-right eat
            newX = x - 1;
            newY = y + direction;
            if(newY >= 0 && newY < BOARD_MATRIX_SIZE && newX >= 0 && \
                    (pressureChecking || \
                    (boardInfo[newX][newY].index != -1 && \
                    chessPieces[boardInfo[newX][newY].index].side() != \
                    piece.side()))){
                moves[moveCount++].setPositions(x, y, newX, newY);
            }

            // w-right & b-left eat
            newX = x + 1;
            if(newY >= 0 && newY < BOARD_MATRIX_SIZE && \
                    newX < BOARD_MATRIX_SIZE && \
                    (pressureChecking || \
                    (boardInfo[newX][newY].index != -1 && \
                    chessPieces[boardInfo[newX][newY].index].side() != \
                    piece.side()))){
                moves[moveCount++].setPositions(x, y, newX, newY);
            }
        }
            break;
        case PIECE_QUEEN:
            moveCount = fillCrossMoves(piece, moves, pressureChecking);
            moveCount += fillStraightMoves(piece, &moves[moveCount], \
                                           pressureChecking);
            break;
        case PIECE_ROOK:
            moveCount = fillStraightMoves(piece, moves, pressureChecking);
            break;
    }

    if(pressureChecking || piece.type() == PIECE_KING){
        return moveCount;
    }

    // if reach here eliminate the king pressure moves
    uint8_t newMoveCount = 0;
    for(uint8_t i = 0; i < moveCount; i++){
        if(checkKingPressure(&moves[i])){
            moves[newMoveCount++] = moves[i];
        }
    }

    return newMoveCount;
}

/*---------------------------------------------------------------------------*/
uint8_t ChessBoard::fillCrossMoves(ChessPiece piece, Move *moves, \
                                   bool pressureChecking)
{
    uint8_t moveCount = 0;
    int8_t x = piece.x();
    int8_t y = piece.y();

    // left up
    for(int8_t i = x - 1, j = y + 1; \
        i >= 0 && j < BOARD_MATRIX_SIZE; i--, j++){
        if(boardInfo[i][j].index != -1){
            if(pressureChecking || \
               chessPieces[boardInfo[i][j].index].side() != piece.side()){
                moves[moveCount++].setPositions(x, y, i, j);
            }
            break;
        }

        moves[moveCount++].setPositions(x, y, i, j);
    }

    // left down
    for(int8_t i = x - 1, j = y - 1; i >= 0 && j >= 0; i--, j--){
        if(boardInfo[i][j].index != -1){
            if(pressureChecking || \
               chessPieces[boardInfo[i][j].index].side() != piece.side()){
                moves[moveCount++].setPositions(x, y, i, j);
            }
            break;
        }

        moves[moveCount++].setPositions(x, y, i, j);
    }

    // right up
    for(int8_t i = x + 1, j = y + 1; \
        i < BOARD_MATRIX_SIZE && j < BOARD_MATRIX_SIZE; i++, j++){
        if(boardInfo[i][j].index != -1){
            if(pressureChecking || \
               chessPieces[boardInfo[i][j].index].side() != piece.side()){
                moves[moveCount++].setPositions(x, y, i, j);
            }
            break;
        }

        moves[moveCount++].setPositions(x, y, i, j);
    }

    // right down
    for(int8_t i = x + 1, j = y - 1; \
        i < BOARD_MATRIX_SIZE && j >= 0; i++, j--){
        if(boardInfo[i][j].index != -1){
            if(pressureChecking || \
               chessPieces[boardInfo[i][j].index].side() != piece.side()){
                moves[moveCount++].setPositions(x, y, i, j);
            }
            break;
        }

        moves[moveCount++].setPositions(x, y, i, j);
    }

    return moveCount;
}

/*---------------------------------------------------------------------------*/
uint8_t ChessBoard::fillStraightMoves(ChessPiece piece, Move *moves, \
                                      bool pressureChecking)
{
    uint8_t moveCount = 0;
    int8_t x = piece.x();
    int8_t y = piece.y();

    // up
    for(int8_t i = x - 1; i >= 0; i--){
        if(boardInfo[i][y].index != -1){
            if(pressureChecking || \
               chessPieces[boardInfo[i][y].index].side() != piece.side()){
                moves[moveCount++].setPositions(x, y, i, y);
            }
            break;
        }

        moves[moveCount++].setPositions(x, y, i, y);
    }

    // down
    for(int8_t i = x + 1; i < BOARD_MATRIX_SIZE; i++){
        if(boardInfo[i][y].index != -1){
            if(pressureChecking || \
               chessPieces[boardInfo[i][y].index].side() != piece.side()){
                moves[moveCount++].setPositions(x, y, i, y);
            }
            break;
        }

        moves[moveCount++].setPositions(x, y, i, y);
    }

    // left
    for(int8_t j = y - 1; j >= 0; j--){
        if(boardInfo[x][j].index != -1){
            if(pressureChecking || \
               chessPieces[boardInfo[x][j].index].side() != piece.side()){
                moves[moveCount++].setPositions(x, y, x, j);
            }
            break;
        }

        moves[moveCount++].setPositions(x, y, x, j);
    }

    // right
    for(int8_t j = y + 1; j < BOARD_MATRIX_SIZE; j++){
        if(boardInfo[x][j].index != -1){
            if(pressureChecking || \
               chessPieces[boardInfo[x][j].index].side() != piece.side()){
                moves[moveCount++].setPositions(x, y, x, j);
            }
            break;
        }

        moves[moveCount++].setPositions(x, y, x, j);
    }

    return moveCount;
}

/*---------------------------------------------------------------------------*/
 void ChessBoard::makeMove(Move move, bool turnSide)
 {
     // clear if any piece exist in next box
     uint8_t currentIndex = boardInfo[move.from.x][move.from.y].index;
     int8_t nextIndex = boardInfo[move.to.x][move.to.y].index;
     if(nextIndex >= 0){
         chessPieces[nextIndex].setOnBoard(false);
     }
     move.setMovedPiece(chessPieces[currentIndex]);
     move.setEatenPieceIndex(nextIndex);

     movePool->push(move);

     // set new position
     chessPieces[currentIndex].setPosition(move.to.x, move.to.y);
     boardInfo[move.to.x][move.to.y].index = currentIndex;

     // set old position as not in use
     boardInfo[move.from.x][move.from.y].index = -1;

     // TODO: if piece is pawn and reached end of the board
     // change piece.

     if(turnSide){
         // turn the side
         movementSide = !movementSide;
     }
 }

/*---------------------------------------------------------------------------*/
void ChessBoard::undoLastMove(bool turnSide)
{
    Move move;
    if(movePool->pop(&move)){
        int8_t movedPieceIndex = boardInfo[move.to.x][move.to.y].index;

        // move back the piece
        chessPieces[movedPieceIndex] = move.movedPiece;
        boardInfo[move.from.x][move.from.y].index = movedPieceIndex;

        // get the eaten piece back if exist
        if(move.pieceWasEaten){
            chessPieces[move.eatenPieceIndex].setOnBoard(true);
            boardInfo[move.to.x][move.to.y].index = move.eatenPieceIndex;
        } else{
            boardInfo[move.to.x][move.to.y].index = -1;
        }

        if(turnSide){
            // turn the side
            movementSide = !movementSide;
        }
    }
}

/*---------------------------------------------------------------------------*/
void ChessBoard::makeAIMove()
{
    minimax(AI_SEARCH_DEPTH, INT_MIN, INT_MAX, true);
    makeMove(bestMove);

    updatePressures();
    this->repaint();

    // is king under pressure check game status
    if(isKingUnderPressure()){
        Move moves[MAX_MOVES_EACH_TURN];
        if(getAllMoves(moves, false) == 0){
            gameOver();
        }
    }
}

/*---------------------------------------------------------------------------*/
int ChessBoard::getRating(bool maximizing)
{
    int sum = 0;
    for(uint8_t i = 0; i < TOTAL_PIECE_NUM; i++){
        if(chessPieces[i].onBoard()){
            sum += chessPieces[i].point();
        }
    }

    return maximizing ? sum : (-1 * sum);
}

/*---------------------------------------------------------------------------*/
int ChessBoard::minimax(int depth, int alpha, int beta, bool maximizing)
{
    if(depth == 0){
        return getRating(maximizing);
    }

    Move moves[MAX_MOVES_EACH_TURN];
    int moveCount = getAllMoves(moves, false);

    if(moveCount == 0){
        if(depth == AI_SEARCH_DEPTH){
            gameOver();
        }
        return getRating(maximizing);
    }

    if(maximizing){
        for(uint8_t i = 0; i < moveCount; i++){
            makeMove(moves[i]);
            int eval = minimax(depth - 1, alpha, beta, false);
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
            int eval = minimax(depth - 1, alpha, beta, true);
            undoLastMove();

            beta = (beta > eval) ? eval : beta;
            if(beta <= alpha) break;
        }
        return beta;
    }
}

/*---------------------------------------------------------------------------*/
void ChessBoard::gameOver()
{
    qDebug() << (movementSide == SIDE_WHITE ? "Black" : "White") << " won!";
}
