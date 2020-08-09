/*
 * ChessBoard Class
 *
 * Copyright (C) Kadir Yanık - <kdrynkk@gmail.com>, 2020
 */
#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "chesspiece.h"
#include "stack.h"
#include "move.h"

#include <QWidget>

/*---------------------------------------------------------------------------*/
#define TOTAL_PIECE_NUM   32
#define MAX_POSSIBLE_MOVE 27 // queen has 27(biggest) legal move
#define BOARD_MATRIX_SIZE 8  // same row & column
#define INVERTING_OFFSET  (BOARD_MATRIX_SIZE - 1)

/*---------------------------------------------------------------------------*/
class ChessBoard : public QWidget
{
    Q_OBJECT
public:
    explicit ChessBoard(QWidget *parent = nullptr);
    ~ChessBoard();
    void paintEvent(QPaintEvent * event);
    void mousePressEvent(QMouseEvent* event);
private:
    void initilizePieces();
    // moving functions
    uint8_t getAllMoves(Move *moves);
    uint8_t prepareLegalMoves(Move *moves);
    uint8_t fillStraightMoves(Move *moves);
    uint8_t fillCrossMoves(Move *moves);
    void makeMove(Move move);
    void undoLastMove();
    // ai functions
    void makeAIMove();
    int getRating();
    int minimax(int depth, int alpha, int beta, bool maximizing);

    Stack<Move> *movePool;
    ChessPiece chessPieces[TOTAL_PIECE_NUM]; // piece properties
    // holds the indexes in chessPieces array
    int8_t boardMatrix[BOARD_MATRIX_SIZE][BOARD_MATRIX_SIZE];

    // array used instead of linked list for improving performance
    Move legalMoves[MAX_POSSIBLE_MOVE];
    int legalMoveCount;
    
    // holds the best move in minimax search
    Move bestMove;

    bool movementSide;
    int8_t selectedIndex;
signals:

};

#endif // CHESSBOARD_H
