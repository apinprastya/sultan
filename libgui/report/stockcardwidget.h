#ifndef STOCKCARDWIDGET_H
#define STOCKCARDWIDGET_H

#include "messagehandler.h"
#include <QWidget>

namespace Ui {
class NormalWidget;
}

namespace LibGUI {

class TableWidget;

class StockCardWidget : public QWidget, public LibG::MessageHandler
{
    Q_OBJECT

public:
    StockCardWidget(LibG::MessageBus *bus, QWidget *parent = 0);
    ~StockCardWidget();
    void showEvent(QShowEvent *e) override;

protected:
    void messageReceived(LibG::Message *msg);

private:
     Ui::NormalWidget *ui;
    TableWidget *mTableWidget;
};

}

#endif // STOCKCARDWIDGET_H
