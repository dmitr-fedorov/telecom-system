#include <QCoreApplication>

#include "../include/deviceemulator.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DeviceEmulator deviceEmulator;
    deviceEmulator.start();

    return a.exec();
}
