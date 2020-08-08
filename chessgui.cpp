/*
 * ChessGui Class
 *
 * Copyright (C) Kadir Yanık - <kdrynkk@gmail.com>, 2020
 */
#include "chessgui.h"
#include "ui_chessgui.h"

#include <QStyle>

/*---------------------------------------------------------------------------*/
ChessGui::ChessGui(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChessGui)
{
    ui->setupUi(this);
    this->setWindowTitle("AI Chess");

    // TODO: select via screen size.
    this->setGeometry(GUI_SIZE_X, 300, GUI_SIZE_X, GUI_SIZE_Y);
    chessBoard = new ChessBoard(this);
}

/*---------------------------------------------------------------------------*/
ChessGui::~ChessGui()
{
    delete chessBoard;
    delete ui;
}

