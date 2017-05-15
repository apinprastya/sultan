#ifndef CATEGOTYADDDIALOG_H
#define CATEGOTYADDDIALOG_H

#include <QDialog>

class QComboBox;

namespace Ui {
class CategoryAddDialog;
}

namespace LibGUI {

class CategoryAddDialog : public QDialog
{
    Q_OBJECT

public:
    CategoryAddDialog(QWidget *parent = 0);
    ~CategoryAddDialog();
    void reset();
    void fill(int id, int parent, const QString &name, const QString &code);
    void enableSaveButton(bool enable);
    QComboBox *getComboParent();

private:
    Ui::CategoryAddDialog *ui;
    int mId;

signals:
    void saveRequest(const QVariantMap &data, int id);

private slots:
    void saveClicked();
};

}
#endif // CATEGOTYADDDIALOG_H
