#ifndef ADDMACHINEDIALOG_H
#define ADDMACHINEDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class AddMachineDialog;
}

namespace LibGUI {

class AddMachineDialog : public QDialog, public LibG::MessageHandler
{
    Q_OBJECT

public:
    AddMachineDialog(LibG::MessageBus *bus, QWidget *parent = 0);
    ~AddMachineDialog();
    void fill(const QVariantMap &data);

protected:
    void messageReceived(LibG::Message *msg);

private:
    Ui::AddMachineDialog *ui;
    int mId = -1;

private slots:
    void saveClicked();
};

}
#endif // ADDMACHINEDIALOG_H
