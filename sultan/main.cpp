#include <QApplication>
#include <QStyleFactory>
#include "core.h"
#include "preference.h"
#include "global_setting_const.h"
#include <QTranslator>
#include <QPalette>
#include <QDir>
#include <QTextStream>
#include <QDebug>

static QTextStream sLogStream;
static int sCounter;

void MessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString formatted = qFormatLogMessage(type, context, msg);
    sLogStream << formatted << "\n";
    if(sCounter++ % 10 == 0)
        sLogStream.flush();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create(QLatin1String("Fusion")));
    a.setApplicationName("sultan");
    a.setOrganizationName("lekapin");
    a.setApplicationVersion("17.07.03");

#ifndef QT_DEBUG
    QDir dir = QDir::home();
    dir.mkdir(".sultan");
    dir.cd(".sultan");
    qSetMessagePattern("%{time yyyy-MM-dd HH:mm:ss} %{type} %{message}");
    qInstallMessageHandler(MessageHandler);
#ifdef Q_OS_WIN32
    QFile file("log.log");
#else
    QFile file(dir.absoluteFilePath("log.log"));
#endif
    file.open(QFile::Append);
    sLogStream.setDevice(&file);
#endif

    QDir appDir(a.applicationDirPath());
    LibG::Preference::createInstance();
    const QString &lang = LibG::Preference::getString(LibG::SETTING::APPLICATION_LANGUAGE, "id");
    QTranslator tr[3];
    if(lang.compare("en")) {
        QStringList trans{"sultan_", "libgui_", "libserver_"};
        for(int i = 0; i < trans.count(); i++) {
            if(tr[i].load(appDir.absoluteFilePath(trans[i] + lang)))
                a.installTranslator(&tr[i]);
        }
    }

    Core core;
    core.setup();

    return a.exec();
}
