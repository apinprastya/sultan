#include "addmachinedialog.h"
#include "ui_addmachinedialog.h"
#include "flashmessagemanager.h"
#include "message.h"
#include "global_constant.h"

using namespace LibGUI;
using namespace LibG;

AddMachineDialog::AddMachineDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddMachineDialog)
{
    ui->setupUi(this);
    setMessageBus(bus);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

AddMachineDialog::~AddMachineDialog()
{
    delete ui;
}

void AddMachineDialog::fill(const QVariantMap &data)
{
    mId = data["id"].toInt();
    ui->lineCode->setText(data["code"].toString());
    ui->lineName->setText(data["name"].toString());
    setWindowTitle(tr("Update machine"));
}

void AddMachineDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isType(MSG_TYPE::MACHINE)) {
        if(msg->isSuccess()) {
            if(msg->isCommand(MSG_COMMAND::INSERT)) FlashMessageManager::showMessage(tr("Machine added successfully"));
            else FlashMessageManager::showMessage(tr("Machine updated successfully"));
            close();
        } else {
            FlashMessageManager::showError(msg->data("error").toString());
        }
        ui->pushSave->setEnabled(true);
    }
}

void AddMachineDialog::saveClicked()
{
    if(ui->lineCode->text().isEmpty() || ui->lineName->text().isEmpty()) {
        FlashMessageManager::showError(tr("Please fill field correctly"));
        return;
    }
    if(ui->lineCode->text().contains(" ")) {
        FlashMessageManager::showError(tr("Code can not contains space"));
        return;
    }
    QVariantMap d{{"code", ui->lineCode->text()}, {"name", ui->lineName->text()}};
    Message msg(MSG_TYPE::MACHINE, MSG_COMMAND::INSERT);
    if(mId <= 0) {
        msg.setData(d);
    } else {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("id", mId);
        msg.addData("data", d);
    }
    sendMessage(&msg);
    ui->pushSave->setEnabled(false);
}
