#include <QApplication>
#include <QStyleFactory>
#include "easylogging++.h"
#include "core.h"
#include "preference.h"
#include "global_setting_const.h"
#include <QTranslator>
#include <QPalette>
#include <QLocale>
#include <QDebug>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create(QLatin1String("Fusion")));
    a.setApplicationVersion("17.06.00");

    LibG::Preference::createInstance();
    const QString &lang = LibG::Preference::getString(LibG::SETTING::APPLICATION_LANGUAGE, "id");
    QTranslator tr[2];
    if(lang.compare("en")) {
        QStringList trans{"sultan_", "libgui_"};
        for(int i = 0; i < trans.count(); i++) {
            tr[i].load(trans[i] + lang);
            a.installTranslator(&tr[i]);
        }
    }

    auto list = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);
    for(auto i : list) {
        QMetaEnum meta = QMetaEnum::fromType<QLocale::Language>();
        qDebug() << meta.valueToKey(i.language());
    }

    Core core;
    core.setup();

    return a.exec();
}
