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
typedef struct {
    int8_t index;
    uint8_t pressure;
} boardInfo_t;

/*---------------------------------------------------------------------------*/
class ChessBoard : public QWidget
{
    Q_OBJECT
public:
    explicit ChessBoard(QWidget *parent = nullptr);
    ~ChessBoard();
    void paintEvent(QPaintEvent * event);
    void mousePressEvent(QMouseEvent* event);

    void undoLastMove(bool turnSide = true);
private:
    void initilizePieces();
    void updatePressures(uint8_t (*pressures)[BOARD_MATRIX_SIZE] = nullptr);
    bool isKingUnderPressure(uint8_t (*pressures)[BOARD_MATRIX_SIZE] = \
            nullptr);
    bool checkKingPressure(Move *move);
    // moving functions
    uint8_t getAllMoves(Move *moves, bool pressureChecking = false);
    uint8_t prepareLegalMoves(ChessPiece piece, Move *moves, \
                              bool pressureChecking = false);
    uint8_t fillStraightMoves(ChessPiece piece, Move *moves, \
                              bool pressureChecking = false);
    uint8_t fillCrossMoves(ChessPiece piece, Move *moves, \
                           bool pressureChecking = false);
    void makeMove(Move move, bool turnSide = true);
    // ai functions
    void makeAIMove();
    int getRating(bool maximizing);
    int minimax(int depth, int alpha, int beta, bool maximizing);

    // notation and game over functions
    QString getNotation(Move *move);
    void gameOver();

    Stack<Move> *movePool;
    ChessPiece chessPieces[TOTAL_PIECE_NUM]; // piece properties
    // holds the indexes in chessPieces array
    boardInfo_t boardInfo[BOARD_MATRIX_SIZE][BOARD_MATRIX_SIZE];

    // array used instead of linked list for improving performance
    Move legalMoves[MAX_POSSIBLE_MOVE];
    int legalMoveCount;
    
    // holds the best move in minimax search
    Move bestMove;

    bool movementSide;
    int8_t selectedIndex;

    // for easy access
    uint8_t whiteKingIndex;
    uint8_t blackKingIndex;
signals:

};

#endif // CHESSBOARD_H
