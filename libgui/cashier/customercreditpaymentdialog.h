#ifndef CUSTOMERCREDITPAYMENTDIALOG_H
#define CUSTOMERCREDITPAYMENTDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class CustomerCreditPaymentDialog;
}

namespace LibGUI {

class CustomerCreditPaymentDialog : public QDialog, public LibG::MessageHandler
{
    Q_OBJECT

public:
    CustomerCreditPaymentDialog(LibG::MessageBus *bus, QWidget *parent = 0);
    ~CustomerCreditPaymentDialog();

protected:
    void messageReceived(LibG::Message *msg) override;

private:
    Ui::CustomerCreditPaymentDialog *ui;
    double mCredit = 0;
    int mId = 0;
    QString mNumber;

    void printData(const QVariantMap &d);

private slots:
    void numberDone();
    void openSearchCustomer();
    void payClicked();
    void calculateRest();
};

}
#endif // CUSTOMERCREDITPAYMENTDIALOG_H
