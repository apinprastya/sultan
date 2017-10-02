#ifndef ADDINGRIDIENTDIALOG_H
#define ADDINGRIDIENTDIALOG_H

#include "messagehandler.h"
#include <QDialog>
#include <QVariantMap>

namespace Ui {
class AddIngridientDialog;
}

namespace LibGUI {

class AddIngridientDialog : public QDialog, public LibG::MessageHandler
{
    Q_OBJECT

public:
    AddIngridientDialog(LibG::MessageBus *bus, QWidget *parent = 0);
    ~AddIngridientDialog();
    void setData(const QVariantMap &data);
    inline QVariantMap &getData() { return mData; }
    inline bool isOk() { return mIsOk; }

protected:
    void messageReceived(LibG::Message *msg) override;
    void fill();

private:
    Ui::AddIngridientDialog *ui;
    double mBuyPrice;
    double mSellPrice;
    bool mIsOk = false;
    QVariantMap mData;

private slots:
    void barcodeClicked();
    void saveClicked();
    void calculatePrice();
};

}
#endif // ADDINGRIDIENTDIALOG_H
