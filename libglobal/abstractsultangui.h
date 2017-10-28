#ifndef ABSTRACTSULTANGUI_H
#define ABSTRACTSULTANGUI_H

#include "global_global.h"
#include <QString>
#include <functional>

namespace LibG {

class GLOBALSHARED_EXPORT AbstractSultanGUI
{
public:
    virtual ~AbstractSultanGUI();
    virtual void showSplashScreen() = 0;
    virtual void hideSplashScreen() = 0;
    virtual void splashShowMessage(const QString &msg) = 0;
    virtual void showSetting() = 0;
    virtual void showMainWindow() = 0;
    virtual void showRestartError(const QString &title, const QString &msg) = 0;
    virtual void guiMessage(int id, const QString &str) = 0;
    virtual void setSettingSocketOpenClose(std::function<void(const QString&, int)> openCon, std::function<void()> closeCon) = 0;
};

}

#endif // ABSTRACTSULTANGUI_H
