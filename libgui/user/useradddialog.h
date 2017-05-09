#ifndef USERADDDIALOG_H
#define USERADDDIALOG_H

#include <QDialog>

namespace Ui {
class UserAddDialog;
}

namespace LibGUI {

class UserAddDialog : public QDialog
{
    Q_OBJECT

public:
    UserAddDialog(QWidget *parent = 0);
    ~UserAddDialog();
    void reset();
    void fill(const QVariantMap &data);
    void enableSave();

private:
    Ui::UserAddDialog *ui;
    int mId = -1;

private slots:
    void saveClicked();

signals:
    void saveData(const QVariantMap &data, int id);
};

}
#endif // USERADDDIALOG_H
