#ifndef REPORTITEMWIDGET_H
#define REPORTITEMWIDGET_H

#include "messagehandler.h"
#include <QWidget>

namespace Ui {
class NormalWidget;
}

namespace LibGUI {

class TableWidget;

class ReportItemWidget : public QWidget, public LibG::MessageHandler
{
    Q_OBJECT
public:
    ReportItemWidget(LibG::MessageBus *bus, QWidget *parent = 0);
    ~ReportItemWidget();

protected:
    void messageReceived(LibG::Message *msg);

private:
    Ui::NormalWidget *ui;
    TableWidget *mTableWidget;
};

}

#endif // REPORTITEMWIDGET_H
