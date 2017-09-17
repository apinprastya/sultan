#ifndef STOCKCARDDIALOG_H
#define STOCKCARDDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class StockCardDialog;
}

namespace LibGUI {

class StockCardDialog : public QDialog
{
    Q_OBJECT

public:
    StockCardDialog(const QString &barcode, LibG::MessageBus *bus, QWidget *parent = 0);
    ~StockCardDialog();
    void showEvent(QShowEvent *e) override;

private:
    Ui::StockCardDialog *ui;
};

}
#endif // STOCKCARDDIALOG_H
