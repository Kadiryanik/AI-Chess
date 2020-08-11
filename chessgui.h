/*
 * ChessGui Class
 *
 * Copyright (C) Kadir Yanık - <kdrynkk@gmail.com>, 2020
 */
#ifndef CHESSGUI_H
#define CHESSGUI_H

#include "chessboard.h"

#include <QWidget>

/*---------------------------------------------------------------------------*/
#define GUI_SIZE_WIDTH  768
#define GUI_SIZE_HEIGHT 512

/*---------------------------------------------------------------------------*/
QT_BEGIN_NAMESPACE
namespace Ui { class ChessGui; }
QT_END_NAMESPACE

/*---------------------------------------------------------------------------*/
class ChessGui : public QWidget
{
    Q_OBJECT

public:
    ChessGui(QWidget *parent = nullptr);
    ~ChessGui();
    void setNotation(QString notation, bool side);

private slots:
    void on_undoButton_clicked();

private:
    Ui::ChessGui *ui;
    ChessBoard *chessBoard;
};
#endif // CHESSGUI_H
