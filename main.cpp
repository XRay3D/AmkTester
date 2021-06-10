#include "devices/devices.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    QApplication::setApplicationName("AmkTest");
    QApplication::setOrganizationName("XrSoft");

    QIcon::setThemeName("breeze");

    [[maybe_unused]] Devices i;

    std::vector<std::unique_ptr<QTranslator>> translators;

    { // NOTE endsWith("/bin") for runinig in project environment
        QDir dir(app.applicationDirPath().endsWith("/bin") ? app.applicationDirPath() + "/../translations"
                                                           : app.applicationDirPath() + "/translations");
        for(auto&& str : dir.entryList({"*ru.qm"}, QDir::Files)) {
            translators.emplace_back(std::make_unique<QTranslator>());
            if(translators.back()->load(str, dir.path()))
                app.installTranslator(translators.back().get());
            else
                qWarning() << QString("QTranslator cant load %1").arg(dir.path());
        }
    }

    MainWindow w;
    w.show();

    return app.exec();
}
