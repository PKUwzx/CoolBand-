#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "KuBand_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.setWindowTitle("CoolBand 酷乐队");
    QIcon windowIcon("C:\\Users\\DamienWang\\Documents\\CoolBand\\icon\\windowIcon.png");
    w.setWindowIcon(windowIcon);
    w.setWindowState(Qt::WindowMaximized);
    w.show();
    return a.exec();
}
