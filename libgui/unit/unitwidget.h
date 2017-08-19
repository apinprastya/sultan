#ifndef UNITWIDGET_H
#define UNITWIDGET_H

#include "messagehandler.h"
#include <QWidget>

namespace Ui {
class NormalWidget;
}

namespace LibGUI {

class TableWidget;

class UnitWidget : public QWidget, public LibG::MessageHandler
{
    Q_OBJECT
public:
    UnitWidget(LibG::MessageBus *bus, QWidget *parent = nullptr);

protected:
    void messageReceived(LibG::Message *msg) override;

private:
    Ui::NormalWidget *ui;
    TableWidget *mTableWidget;

private slots:
    void addClicked();
    void editClicked(const QModelIndex &index);
    void deleteClicked(const QModelIndex &index);
};

}

#endif // UNITWIDGET_H
