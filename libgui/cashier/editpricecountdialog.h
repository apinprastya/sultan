#ifndef EDITPRICECOUNTDIALOG_H
#define EDITPRICECOUNTDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class EditPriceCountDialog;
}

namespace LibGUI {

class EditPriceCountDialog : public QDialog, public LibG::MessageHandler
{
    Q_OBJECT

public:
    EditPriceCountDialog(LibG::MessageBus *bus, QWidget *parent = 0);
    ~EditPriceCountDialog();
    void setup(const QString &barcode, float count, double price, const QString disc);
    inline bool isOk() { return mIsOk; }
    float getCount();
    double getPrice();
    QString getDiscountFormula();

protected:
    void messageReceived(LibG::Message *msg) override;

private:
    Ui::EditPriceCountDialog *ui;
    bool mIsOk = false;

private slots:
    void updatePrice();
    void saveClicked();
};

}
#endif // EDITPRICECOUNTDIALOG_H
