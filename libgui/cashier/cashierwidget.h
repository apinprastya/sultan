#ifndef CASHIERWIDGET_H
#define CASHIERWIDGET_H

#include "messagehandler.h"
#include <QWidget>

namespace Ui {
class CashierWidget;
}

namespace LibGUI {

class CashierTableModel;

class CashierWidget : public QWidget, public LibG::MessageHandler
{
    Q_OBJECT

public:
    CashierWidget(LibG::MessageBus *bus, QWidget *parent = 0);
    ~CashierWidget();
    void showEvent(QShowEvent *event) override;

protected:
    void messageReceived(LibG::Message *msg) override;

private:
    Ui::CashierWidget *ui;
    float mCount = 0.0f;
    CashierTableModel *mModel;

private slots:
    void barcodeEntered();
    void totalChanged(double value);
    void selectRow(const QModelIndex &index);
    void tableKeyPressed(QObject *sender, QKeyEvent *event);
};

}
#endif // CASHIERWIDGET_H
