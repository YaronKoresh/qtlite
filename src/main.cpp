#include <QApplication>
#include <QCoreApplication>
#include "ui/qtliteapp.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setOrganizationName("YaronKoresh");
    QCoreApplication::setApplicationName("QtLite");
    QCoreApplication::setApplicationVersion("1.0.0");

    QApplication app(argc, argv);

    QtLiteApp window;
    window.show();

    return app.exec();
}
