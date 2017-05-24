#ifndef RESTARTCONFIRMATIONDIALOG_H
#define RESTARTCONFIRMATIONDIALOG_H

#include <QDialog>

namespace Ui {
class RestartConfirmationDialog;
}

class RestartConfirmationDialog : public QDialog
{
    Q_OBJECT

public:
    RestartConfirmationDialog(QWidget *parent = 0);
    ~RestartConfirmationDialog();
    void setMessage(const QString &title, const QString &msg);

private:
    Ui::RestartConfirmationDialog *ui;

private slots:
    void restartClicked();
    void exitClicked();
};

#endif // RESTARTCONFIRMATIONDIALOG_H
