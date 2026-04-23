#include <QApplication>
#include "ui/MainWindow.h"
#include "utils/FileConstants.h"
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QIcon>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QApplication::setApplicationName(Config::AppName);
    QApplication::setOrganizationName(Config::Organization);
    QApplication::setApplicationVersion("1.1.0");

    QLocale sysLocale = QLocale::system();

    QTranslator qtTr;
    if (qtTr.load(sysLocale, "qtbase", "_",
                  QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        app.installTranslator(&qtTr);
                  }

    QTranslator appTr;
    if (sysLocale.name().startsWith("es")) {
        if (appTr.load(":/i18n/gluefiles_es.qm") || appTr.load(":/gluefiles_es.qm")) {
            app.installTranslator(&appTr);
        }
    }

    app.setWindowIcon(QIcon(":/icons/icon.svg"));

    MainWindow w;
    w.show();

    return app.exec();
}