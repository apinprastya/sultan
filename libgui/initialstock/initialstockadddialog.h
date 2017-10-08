#ifndef INITIALSTOCKADDDIALOG_H
#define INITIALSTOCKADDDIALOG_H

#include "messagehandler.h"
#include <QDialog>
#include <QNetworkReply>

class QNetworkAccessManager;

namespace Ui {
class InitialStockAddDialog;
}

namespace LibGUI {

class InitialStockAddDialog : public QDialog, public LibG::MessageHandler
{
    Q_OBJECT

public:
    InitialStockAddDialog(LibG::MessageBus *bus, QWidget *parent = 0);
    ~InitialStockAddDialog();
    void reset();

protected:
    void messageReceived(LibG::Message *msg) override;
    void showEvent(QShowEvent *event) override;

private:
    Ui::InitialStockAddDialog *ui;
    bool mIsAddAgain = false;
    bool mIsReqCloud = false;
    int mCurrentCategory = 0;
    int mCurrentSuplier = 0;
    QNetworkAccessManager *mNetworkManager;
    QString mLastBarcode;

    void openAddItem();
    void save();

private slots:
    void barcodeDone();
    void addClicked();
    void addAgainClicked();
    void checkDone();
    void httpError(QNetworkReply::NetworkError error);
    void calculateMargin();

signals:
    void addSuccess();
};

}
#endif // INITIALSTOCKADDDIALOG_H
