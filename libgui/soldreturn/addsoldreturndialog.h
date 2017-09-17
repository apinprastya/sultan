#ifndef ADDSOLDRETURNDIALOG_H
#define ADDSOLDRETURNDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class AddSoldReturnDialog;
}

namespace LibGUI {

class AddSoldReturnDialog : public QDialog, public LibG::MessageHandler
{
    Q_OBJECT

public:
    AddSoldReturnDialog(LibG::MessageBus *bus, QWidget *parent = 0);
    ~AddSoldReturnDialog();
    void fill(const QVariantMap &data);

protected:
    void messageReceived(LibG::Message *msg) override;

private:
    Ui::AddSoldReturnDialog *ui;
    int mId = -1;
    int mSoldId = -1;
    int mSoldItemId = -1;
    int mBankSelected = -1;
    double mPrice = 0;

    void fillField(const QVariantMap &data);

private slots:
    void saveClicked();
    void calculatePrice();
    void openTransactionDialog();
    void checkBank();
};

}

#endif // ADDSOLDRETURNDIALOG_H
