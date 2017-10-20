#ifndef CASHIERREPORTDIALOG_H
#define CASHIERREPORTDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class CashierReportDialog;
}

namespace LibGUI {

class CashierReportDialog : public QDialog, public LibG::MessageHandler
{
    Q_OBJECT

public:
    CashierReportDialog(LibG::MessageBus *bus, QWidget *parent = 0);
    ~CashierReportDialog();

protected:
    void messageReceived(LibG::Message *msg) override;

private:
    Ui::CashierReportDialog *ui;
    QVariantList mData;

    QString getType(int type);

private slots:
    void search();
    void print();
};

}
#endif // CASHIERREPORTDIALOG_H
