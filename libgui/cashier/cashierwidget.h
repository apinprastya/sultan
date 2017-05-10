#ifndef CASHIERWIDGET_H
#define CASHIERWIDGET_H

#include "messagehandler.h"
#include <QWidget>

namespace Ui {
class CashierWidget;
}

namespace LibGUI {

class CashierWidget : public QWidget, public LibG::MessageHandler
{
    Q_OBJECT

public:
    CashierWidget(LibG::MessageBus *bus, QWidget *parent = 0);
    ~CashierWidget();

protected:
    void messageReceived(LibG::Message *msg) override;

private:
    Ui::CashierWidget *ui;
};

}
#endif // CASHIERWIDGET_H
