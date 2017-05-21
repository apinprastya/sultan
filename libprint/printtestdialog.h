#ifndef PRINTTESTDIALOG_H
#define PRINTTESTDIALOG_H

#include "print_global.h"
#include <QDialog>

namespace Ui {
class PrintTestDialog;
}

namespace LibPrint {

class PRINTSHARED_EXPORT PrintTestDialog : public QDialog
{
    Q_OBJECT

public:
    PrintTestDialog(QWidget *parent = 0);
    ~PrintTestDialog();

private:
    Ui::PrintTestDialog *ui;

private slots:
    void printClicked();
    void updateEnable();
};

}
#endif // PRINTTESTDIALOG_H
