#include "chessgui.h"

#include <QApplication>

/*---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChessGui w;
    w.show();
    return a.exec();
}
