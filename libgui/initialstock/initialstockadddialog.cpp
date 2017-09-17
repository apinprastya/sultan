#include "initialstockadddialog.h"
#include "ui_initialstockadddialog.h"
#include "keyevent.h"
#include "guiutil.h"
#include "message.h"
#include "messagebus.h"
#include "global_constant.h"
#include "usersession.h"
#include "flashmessagemanager.h"
#include "guiutil.h"
#include "item/additemdialog.h"
#include "preference.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

using namespace LibGUI;
using namespace LibG;


InitialStockAddDialog::InitialStockAddDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InitialStockAddDialog),
    mNetworkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    setMessageBus(bus);
    auto ke = new KeyEvent(this);
    ke->addConsumeKey(Qt::Key_Return);
    ke->addConsumeKey(Qt::Key_Enter);
    ke->addConsumeKey(Qt::Key_Tab);
    ui->lineBarcode->installEventFilter(ke);
    connect(ke, SIGNAL(keyPressed(QObject*,QKeyEvent*)), SLOT(barcodeDone()));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(addClicked()));
    connect(ui->pushSaveAgain, SIGNAL(clicked(bool)), SLOT(addAgainClicked()));
    connect(ui->doubleSellPrice, SIGNAL(valueChanged(double)), SLOT(calculateMargin()));
    connect(ui->doubleBuyPrice, SIGNAL(valueChanged(double)), SLOT(calculateMargin()));
    ui->checkOnline->hide();
}

InitialStockAddDialog::~InitialStockAddDialog()
{
    delete ui;
}

void InitialStockAddDialog::reset()
{
    ui->lineBarcode->clear();
    ui->lineBarcode->setFocus(Qt::TabFocusReason);
    ui->doubleStock->setValue(0);
    ui->doubleBuyPrice->setValue(0);
    ui->doubleSellPrice->setValue(0);
    ui->labelName->clear();
    GuiUtil::enableWidget(false, QList<QWidget*>() << ui->doubleBuyPrice << ui->doubleSellPrice << ui->doubleStock <<
                          ui->comboCategory << ui->comboSuplier << ui->pushSave << ui->pushSaveAgain);
}

void InitialStockAddDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::GET)) {
        if(msg->isSuccess()) {
            if(msg->data("stock").toFloat() != 0) {
                QMessageBox::critical(this, tr("Error"), tr("Item already has stock. Use checkstock if you want change the stock"));
            } else {
                GuiUtil::enableWidget(true, QList<QWidget*>() << ui->doubleBuyPrice << ui->doubleSellPrice << ui->doubleStock <<
                                      ui->comboCategory << ui->comboSuplier << ui->pushSave << ui->pushSaveAgain);
                ui->labelName->setText(msg->data("name").toString());
                ui->comboSuplier->setFocus(Qt::TabFocusReason);
                GuiUtil::selectCombo(ui->comboCategory, msg->data("category_id"));
                GuiUtil::selectCombo(ui->comboSuplier, msg->data("suplier_id"));
                ui->doubleBuyPrice->setValue(msg->data("buy_price").toDouble());
                ui->doubleSellPrice->setValue(msg->data("sell_price").toDouble());
            }
        } else if(ui->checkOnline->isChecked()) {
            QNetworkRequest request(QUrl(LibG::CONSTANT::BARCODE_CLOUD.arg(mLastBarcode)));
            auto reply = mNetworkManager->get(request);
            connect(reply, SIGNAL(finished()), SLOT(checkDone()));
            connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(httpError(QNetworkReply::NetworkError)));
        } else {
            /*if(!ui->checkOnline->isChecked())
                QMessageBox::critical(this, tr("Error"), tr("Item not found. You can check item online by tick the \"Check Online\" checkbox"));*/
            openAddItem();
        }
    } else if(msg->isType(MSG_TYPE::SUPLIER)) {
        const QVariantList &list = msg->data("data").toList();
        GuiUtil::populateCombo(ui->comboSuplier, list, tr("-- Select Suplier --"));
        GuiUtil::selectCombo(ui->comboSuplier, mCurrentSuplier);
    } else if(msg->isType(MSG_TYPE::CATEGORY)) {
        const QVariantList &list = msg->data("data").toList();
        GuiUtil::populateCombo(ui->comboCategory, list, tr("-- Select Category --"));
        GuiUtil::selectCombo(ui->comboCategory, mCurrentCategory);
    } else if(msg->isTypeCommand(MSG_TYPE::CHECKSTOCK, MSG_COMMAND::INSERT)) {
        if(msg->isSuccess()) {
            FlashMessageManager::showMessage(tr("Initial stock addedd successfully"));
            if(mIsAddAgain) reset();
            else close();
            emit addSuccess();
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
        }
    }
}

void InitialStockAddDialog::showEvent(QShowEvent *event)
{
    Message msg(MSG_TYPE::SUPLIER, MSG_COMMAND::QUERY);
    msg.setLimit(-1);
    sendMessage(&msg);
    Message msg2(MSG_TYPE::CATEGORY, MSG_COMMAND::QUERY);
    msg2.setLimit(-1);
    sendMessage(&msg2);
    QDialog::showEvent(event);
}

void InitialStockAddDialog::openAddItem()
{
    AddItemDialog dialog(mMessageBus, this);
    dialog.reset(false);
    dialog.disableAddAgain();
    dialog.setBarcode(ui->lineBarcode->text());
    dialog.exec();
    if(dialog.isSuccess()) barcodeDone();
}

void InitialStockAddDialog::save()
{
    if(mLastBarcode.isEmpty()) return;
    if(mLastBarcode != ui->lineBarcode->text()) {
        QMessageBox::critical(this, tr("Error"), tr("Please redo the process"));
        return;
    }
    ui->pushSave->setEnabled(false);
    ui->pushSaveAgain->setEnabled(false);
    Message msg(MSG_TYPE::CHECKSTOCK, MSG_COMMAND::INSERT);
    msg.addData("barcode", mLastBarcode);
    msg.addData("name", ui->labelName->text());
    msg.addData("system_stock", 0);
    msg.addData("real_stock", ui->doubleStock->value());
    msg.addData("user_id", UserSession::id());
    msg.addData("buy_price", ui->doubleBuyPrice->value());
    msg.addData("total_price", ui->doubleBuyPrice->value() * (ui->doubleStock->value() - ui->doubleStock->value()));
    msg.addData("flag", CHECKSTOCK_FLAG::INITIAL);
    msg.addData("suplier_id", ui->comboSuplier->currentData());
    msg.addData("category_id", ui->comboCategory->currentData());
    msg.addData("sell_price", ui->doubleSellPrice->value());
    sendMessage(&msg);
}

void InitialStockAddDialog::barcodeDone()
{
    mLastBarcode = ui->lineBarcode->text();
    if(mLastBarcode.isEmpty()) return;
    Message msg(MSG_TYPE::ITEM, MSG_COMMAND::GET);
    msg.addData("barcode", mLastBarcode);
    sendMessage(&msg);
    GuiUtil::enableWidget(false, QList<QWidget*>() << ui->doubleBuyPrice << ui->doubleSellPrice << ui->doubleStock <<
                          ui->comboCategory << ui->comboSuplier << ui->pushSave << ui->pushSaveAgain);
}

void InitialStockAddDialog::addClicked()
{
    mIsAddAgain = false;
    save();
}

void InitialStockAddDialog::addAgainClicked()
{
    mIsAddAgain = true;
    save();
}

void InitialStockAddDialog::checkDone()
{
    auto reply = static_cast<QNetworkReply*>(QObject::sender());
    auto json = QJsonDocument::fromJson(reply->readAll()).object();
    if(json.contains("name")) {
        GuiUtil::enableWidget(true, QList<QWidget*>() << ui->doubleBuyPrice << ui->doubleSellPrice << ui->doubleStock <<
                              ui->comboCategory << ui->comboSuplier << ui->pushSave << ui->pushSaveAgain);
        ui->labelName->setText(json.value("name").toString());
        ui->comboSuplier->setFocus(Qt::TabFocusReason);
    } else {
        openAddItem();
    }
    reply->deleteLater();
}

void InitialStockAddDialog::httpError(QNetworkReply::NetworkError /*error*/)
{
    auto reply = static_cast<QNetworkReply*>(QObject::sender());
    QMessageBox::critical(this, tr("Error"), reply->errorString());
    reply->deleteLater();
}

void InitialStockAddDialog::calculateMargin()
{
    auto buy = ui->doubleBuyPrice->value();
    auto sell = ui->doubleSellPrice->value();
    double c = buy == 0 ? 0 : (sell-buy) * 100 / buy;
    ui->labelMargin->setText(QString("Margin : %1 (%2 %)").arg(Preference::formatMoney(sell - buy)).arg(QString::number(c, 'f', 2)));
}
