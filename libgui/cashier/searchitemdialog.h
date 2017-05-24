#ifndef SEARCHITEMDIALOG_H
#define SEARCHITEMDIALOG_H

#include <QDialog>

namespace Ui {
class SearchItemDialog;
}

namespace LibG {
class MessageBus;
}

namespace LibGUI {

class TableWidget;

class SearchItemDialog : public QDialog
{
    Q_OBJECT

public:
    SearchItemDialog(LibG::MessageBus *bus, QWidget *parent = 0);
    ~SearchItemDialog();
    inline QString getSelectedBarcode() { return mSelectedBarcode; }

private:
    Ui::SearchItemDialog *ui;
    TableWidget *mTableWidget;
    QString mSelectedBarcode;

private slots:
    void nameDone();
    void dataLoaded();
    void returnPressed();
};

}
#endif // SEARCHITEMDIALOG_H
