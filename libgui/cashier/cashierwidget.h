#ifndef CASHIERWIDGET_H
#define CASHIERWIDGET_H

#include "messagehandler.h"
#include <QWidget>

namespace Ui {
class CashierWidget;
}

namespace LibGUI {

class CashierTableModel;
class PayCashDialog;

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
    PayCashDialog *mPayCashDialog;

private slots:
    void barcodeEntered();
    void totalChanged(double value);
    void selectRow(const QModelIndex &index);
    void tableKeyPressed(QObject *sender, QKeyEvent *event);
    void payCash();
    void payCashless();
    void openDrawer();
    void updateLastInputed();
    void payCashRequested(double value);
    void printBill(const QVariantMap &data);
};

}
#endif // CASHIERWIDGET_H
