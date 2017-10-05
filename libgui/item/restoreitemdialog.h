#ifndef RESTOREITEMDIALOG_H
#define RESTOREITEMDIALOG_H

#include <QDialog>

namespace Ui {
class RestoreItemDialog;
}

namespace LibGUI {

class RestoreItemDialog : public QDialog
{
    Q_OBJECT

public:
    RestoreItemDialog(const QVariantMap &data, QWidget *parent = 0);
    ~RestoreItemDialog();
    inline bool isOk() { return mIsOk; }

private:
    Ui::RestoreItemDialog *ui;
    bool mIsOk = false;

private slots:
    void restoreClicked();
};

}
#endif // RESTOREITEMDIALOG_H
