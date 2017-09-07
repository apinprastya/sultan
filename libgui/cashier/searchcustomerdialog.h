#ifndef SEARCHCUSTOMERDIALOG_H
#define SEARCHCUSTOMERDIALOG_H

#include <QDialog>
#include <QVariantMap>

namespace Ui {
class SearchCustomerDialog;
}

namespace LibG {
class MessageBus;
}

namespace LibGUI {

class SearchCustomerDialog : public QDialog
{
    Q_OBJECT

public:
    SearchCustomerDialog(LibG::MessageBus *bus, QWidget *parent = 0);
    ~SearchCustomerDialog();
    inline QVariantMap getSelectedData() { return mSelectedData; }
    inline bool isOk() { return mIsOk; }

private:
    Ui::SearchCustomerDialog *ui;
    QVariantMap mSelectedData;
    bool mIsOk = false;

private slots:
    void nameDone();
    void dataLoaded();
    void returnPressed();
    void doubleClicked(const QModelIndex &index);
};

}

#endif // SEARCHCUSTOMERDIALOG_H
