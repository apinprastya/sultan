#ifndef IMPORTITEMDIALOG_H
#define IMPORTITEMDIALOG_H

#include <QDialog>

namespace Ui {
class ImportItemDialog;
}

namespace LibGUI {

class ImportItemDialog : public QDialog {
    Q_OBJECT

  public:
    ImportItemDialog(QWidget *parent = nullptr);
    ~ImportItemDialog();
    inline bool isChecked() { return mIsChecked; }

  private slots:
    void on_pushImport_clicked();

  private:
    Ui::ImportItemDialog *ui;
    bool mIsChecked = false;
};

} // namespace LibGUI

#endif // IMPORTITEMDIALOG_H
