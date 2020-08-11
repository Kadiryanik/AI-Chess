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

    ui->notationTable->setRowCount(0);
    ui->notationTable->setColumnCount(2);
    ui->notationTable->setHorizontalHeaderItem(0, \
                                               new QTableWidgetItem("White"));
    ui->notationTable->setHorizontalHeaderItem(1, \
                                               new QTableWidgetItem("Black"));
}

/*---------------------------------------------------------------------------*/
ChessGui::~ChessGui()
{
    delete chessBoard;
    delete ui;
}

/*---------------------------------------------------------------------------*/
void ChessGui::setNotation(QString notation, bool side)
{
    if(side == SIDE_WHITE){
        int row = ui->notationTable->rowCount();
        ui->notationTable->setRowCount(row + 1);
        QTableWidgetItem *item = new QTableWidgetItem(notation);
        ui->notationTable->setItem(row, 0, item);
    } else{
        QTableWidgetItem *item = new QTableWidgetItem(notation);
        ui->notationTable->setItem(ui->notationTable->rowCount() - 1, 1, \
                                   item);
    }

    ui->notationTable->scrollToBottom();
}

/*---------------------------------------------------------------------------*/
void ChessGui::on_undoButton_clicked()
{
    chessBoard->undoLastMove(); // black
    chessBoard->undoLastMove(); // white
    chessBoard->repaint();
    ui->notationTable->removeRow(ui->notationTable->rowCount() - 1);
}
