#include "mainwindow.h"
#include <spdlog/spdlog.h>
#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QStyleFactory>
#include <spdlog/sinks/ansicolor_sink.h>
#include "settingsvalues.h"

constexpr static const char* DEFAULT_THEME = "Dark";

constexpr static const char* DARK_THEME_PATH = ":/qss/generalDark.qss";
constexpr static const char* LIGHT_THEME_PATH = ":/qss/generalLight.qss";

constexpr static const char* ORG_NAME = "klewy";
constexpr static const char* ORG_DOMAIN = "klewy.com";
constexpr static const char* APP_NAME = "MusViz";

namespace { // internal linkage cpp way
void loadThemes(QApplication& app) {
    QSettings settings;
    QString themeStr = settings.value(SettingsValues::THEME, DEFAULT_THEME).toString();
    QString themeLoc = themeStr == DEFAULT_THEME ? DARK_THEME_PATH : LIGHT_THEME_PATH;

    QFile file(themeLoc);
    if (file.open(QIODevice::ReadOnly)) {
        app.setStyleSheet(file.readAll());
        file.close();
    } else {
        spdlog::error("Could not read theme: {}", file.errorString().toStdString());
    }
    app.setStyle(QStyleFactory::create("Fusion"));
    spdlog::info("Theme is loaded");
}
}

int main(int argc, char *argv[])
{
    spdlog::info("App is starting");

    QCoreApplication::setOrganizationName(ORG_NAME);
    QCoreApplication::setOrganizationDomain(ORG_DOMAIN);
    QCoreApplication::setApplicationName(APP_NAME);

    QApplication a(argc, argv);

    loadThemes(a);

    MainWindow w;
    w.show();
    return a.exec();
}
