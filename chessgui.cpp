/*
 * ChessGui Class
 *
 * Copyright (C) Kadir Yanık - <kdrynkk@gmail.com>, 2020
 */
#include "chessgui.h"
#include "ui_chessgui.h"


#include <QDesktopWidget>
#include <QSizePolicy>
#include <QStyle>

/*---------------------------------------------------------------------------*/
ChessGui::ChessGui(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChessGui)
{
    ui->setupUi(this);
    this->setWindowTitle("AI Chess");

    // TODO: convert screenGeometry to QGuiApplication::screens()
    QRect rec = QApplication::desktop()->screenGeometry();
    this->setGeometry((rec.width() / 2) - GUI_SIZE_WIDTH / 2, \
                      (rec.height() / 2) - GUI_SIZE_HEIGHT / 2, \
                      GUI_SIZE_WIDTH, GUI_SIZE_HEIGHT);
    this->setFixedSize(this->geometry().width(), this->geometry().height());

    chessBoard = new ChessBoard(this);
}

/*---------------------------------------------------------------------------*/
ChessGui::~ChessGui()
{
    delete chessBoard;
    delete ui;
}

