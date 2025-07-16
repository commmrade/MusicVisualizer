#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("klewy");
    QCoreApplication::setOrganizationDomain("klewy.com");
    QCoreApplication::setApplicationName("MusViz");

    QApplication a(argc, argv);

    QSettings settings;
    QString themeStr = settings.value("gui/theme", "Dark").toString();
    QString themeLoc = themeStr == "Dark" ? ":/qss/generalDark.qss" : ":/qss/generalLight.qss";


    QFile file(themeLoc);
    if (file.open(QIODevice::ReadOnly)) {
        a.setStyleSheet(file.readAll());
        file.close();
    } else {
        qDebug() << themeLoc;
        qDebug() << "could not open theme" << file.errorString();
    }
    a.setStyle(QStyleFactory::create("Fusion"));


    MainWindow w;
    w.show();
    return a.exec();
}
