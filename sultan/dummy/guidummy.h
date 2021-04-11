#ifndef GUIDUMMY_H
#define GUIDUMMY_H

#include "abstractsultangui.h"

class GuiDummy : public LibG::AbstractSultanGUI {
  public:
    GuiDummy();
    void showSplashScreen() override;
    void hideSplashScreen() override;
    void splashShowMessage(const QString &msg) override;
    void showSetting() override;
    void showMainWindow() override;
    void showRestartError(const QString &title, const QString &msg) override;
    void guiMessage(int id, const QString &str) override;
    void setSettingSocketOpenClose(std::function<void(const QString &, int)> openCon,
                                   std::function<void()> closeCon) override;
};

#endif // GUIDUMMY_H
