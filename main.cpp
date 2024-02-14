#include "ClientServer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClientServer w;
    w.show();
    return a.exec();
}
