#include "unitwidget.h"
#include "ui_normalwidget.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "global_constant.h"
#include "tableitem.h"
#include "message.h"
#include "guiutil.h"
#include "tableview.h"
#include "headerwidget.h"
#include "flashmessagemanager.h"
#include "util.h"
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>

using namespace LibGUI;
using namespace LibG;

UnitWidget::UnitWidget(LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalWidget),
    mTableWidget(new TableWidget(this))
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->labelTitle->setText(tr("Unit"));
    ui->verticalLayout->addWidget(mTableWidget);
    mTableWidget->initCrudButton();
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("name", tr("Name"));
    model->addHeaderFilter("name", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});;
    model->setTypeCommand(MSG_TYPE::UNIT, MSG_COMMAND::QUERY);
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    model->refresh();
    connect(mTableWidget, SIGNAL(addClicked()), SLOT(addClicked()));
    connect(mTableWidget, SIGNAL(updateClicked(QModelIndex)), SLOT(editClicked(QModelIndex)));
    connect(mTableWidget, SIGNAL(deleteClicked(QModelIndex)), SLOT(deleteClicked(QModelIndex)));
    //connect(mAddDialog, SIGNAL(saveData(QVariantMap,int)), SLOT(saveRequested(QVariantMap,int)));
}

void UnitWidget::messageReceived(LibG::Message *msg)
{
    if(msg->isSuccess()) {
        if(msg->isType(MSG_TYPE::UNIT)) {
            if(msg->isCommand(MSG_COMMAND::INSERT)) {
                FlashMessageManager::showMessage(tr("Unit added successfully"));
            } else if(msg->isCommand(MSG_COMMAND::UPDATE)) {
                FlashMessageManager::showMessage(tr("Unit updated successfully"));
            } else {
                FlashMessageManager::showMessage(tr("Unit deleted successfully"));
            }
            mTableWidget->getModel()->refresh();
        }
    } else {
        FlashMessageManager::showError(msg->data("error").toString());
    }
}

void UnitWidget::addClicked()
{
    const QString &name = QInputDialog::getText(this, tr("Add new unit"), tr("Input name"));
    if(!name.isEmpty()) {
        Message msg(MSG_TYPE::UNIT, MSG_COMMAND::INSERT);
        msg.addData("name", Util::capitalize(name));
        sendMessage(&msg);
    }
}

void UnitWidget::editClicked(const QModelIndex &index)
{
    auto item = static_cast<TableItem*>(index.internalPointer());
    const QString &name = QInputDialog::getText(this, tr("Add new unit"), tr("Input name"), QLineEdit::Normal, item->data("name").toString());
    if(!name.isEmpty()) {
        Message msg(MSG_TYPE::UNIT, MSG_COMMAND::UPDATE);
        msg.addData("id", item->id);
        msg.addData("data", QVariantMap{{"name", Util::capitalize(name)}});
        sendMessage(&msg);
    }
}

void UnitWidget::deleteClicked(const QModelIndex &index)
{
    auto item = static_cast<TableItem*>(index.internalPointer());
    int ret = QMessageBox::question(this, tr("Confirmation"), tr("Are you sure want to remove unit?"));
    if(ret == QMessageBox::Yes) {
        Message msg(MSG_TYPE::UNIT, MSG_COMMAND::DEL);
        msg.addData("id", item->id);
        sendMessage(&msg);
    }
}
