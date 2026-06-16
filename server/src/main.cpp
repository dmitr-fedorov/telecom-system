#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>

#include "../../shared/include/types.h"
#include "../include/mainwindow.h"
#include "../include/types.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  qRegisterMetaType<server::types::ClientInfo>("server::types::ClientInfo");
  qRegisterMetaType<server::types::ClientData>("server::types::ClientData");
  qRegisterMetaType<shared::types::LimitsConfig>("shared::types::LimitsConfig");

  QTranslator translator;

  if (translator.load("qtbase_ru",
                      QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
    a.installTranslator(&translator);
  }

  server::MainWindow w;
  w.show();
  return a.exec();
}
