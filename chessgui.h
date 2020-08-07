#ifndef CHESSGUI_H
#define CHESSGUI_H

#include "chessboard.h"

#include <QWidget>

/*---------------------------------------------------------------------------*/
#define GUI_SIZE_X 512
#define GUI_SIZE_Y 512

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

private:
    Ui::ChessGui *ui;
    ChessBoard* chessBoard;
};
#endif // CHESSGUI_H
