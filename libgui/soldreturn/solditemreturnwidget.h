#ifndef SOLDITEMRETURNWIDGET_H
#define SOLDITEMRETURNWIDGET_H

#include "messagehandler.h"
#include <QWidget>

namespace Ui {
class NormalWidget;
}

namespace LibGUI {

class TableWidget;
class TileWidget;

class SoldItemReturnWidget : public QWidget, public LibG::MessageHandler
{
    Q_OBJECT
public:
    SoldItemReturnWidget(LibG::MessageBus *bus, QWidget *parent = 0);

protected:
    void messageReceived(LibG::Message *msg) override;

private:
    Ui::NormalWidget *ui;
    TableWidget *mTableWidget;
    TileWidget *mTotalDebt;

private slots:
    void addClicked();
    void updateClicked(const QModelIndex &index);
    void deleteClicked(const QModelIndex &index);
    void getSummary();
};

}

#endif // SOLDITEMRETURNWIDGET_H
