#include <QCoreApplication>

#include "../include/deviceemulator.h"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  qRegisterMetaType<shared::types::LimitsConfig>("shared::types::LimitsConfig");

  setlocale(LC_ALL, "Russian");

  client::DeviceEmulator deviceEmulator;
  deviceEmulator.start();

  return a.exec();
}
