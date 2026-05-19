#include <QCoreApplication>

#include "../include/clientapp.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ClientApp clientApp;
    clientApp.start();

    return a.exec();
}
