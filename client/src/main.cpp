#include <QCoreApplication>

#include "../include/deviceemulator.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qRegisterMetaType<sharedTypes::LimitsConfig>(
        "sharedTypes::LimitsConfig");

    DeviceEmulator deviceEmulator;
    deviceEmulator.start();

    return a.exec();
}
