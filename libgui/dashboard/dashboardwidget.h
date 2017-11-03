#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include "messagehandler.h"
#include <QWidget>

namespace Ui {
class DashboardWidget;
}

namespace LibGUI {

class TileWidget;

class DashboardWidget : public QWidget, public LibG::MessageHandler
{
    Q_OBJECT

public:
    DashboardWidget(LibG::MessageBus *bus, QWidget *parent = 0);
    ~DashboardWidget();

protected:
    void messageReceived(LibG::Message *msg) override;

private:
    Ui::DashboardWidget *ui;
    TileWidget *mStockValue;
    TileWidget *mDebt;
    TileWidget *mCredit;
    TileWidget *mSales;
    TileWidget *mMargin;
};

}

#endif // DASHBOARDWIDGET_H
