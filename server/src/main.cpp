#include "../include/mainwindow.h"
#include "../../shared/include/sharedtypes.h"

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<sharedTypes::LimitsConfig>(
        "sharedTypes::LimitsConfig");

    QTranslator translator;

    if (translator.load(
            "qtbase_ru",
            QLibraryInfo::path(
                QLibraryInfo::TranslationsPath)))
    {
        a.installTranslator(&translator);
    }

    MainWindow w;
    w.show();
    return a.exec();
}
