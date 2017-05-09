#include "useradddialog.h"
#include "ui_useradddialog.h"
#include "guiutil.h"
#include <QCryptographicHash>
#include <QMessageBox>

using namespace LibGUI;

UserAddDialog::UserAddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserAddDialog)
{
    ui->setupUi(this);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

UserAddDialog::~UserAddDialog()
{
    delete ui;
}

void UserAddDialog::reset()
{
    ui->lineName->clear();
    ui->linePassword->clear();
    ui->linePhone->clear();
    ui->lineRepassword->clear();
    ui->lineUsername->clear();
    ui->textAddress->clear();
    ui->lineUsername->setFocus(Qt::TabFocusReason);
    ui->pushSave->setEnabled(true);
    setWindowTitle(tr("Add new user"));
}

void UserAddDialog::fill(const QVariantMap &data)
{
    ui->lineName->setText(data["name"].toString());
    ui->linePhone->setText(data["phone"].toString());
    ui->textAddress->setPlainText(data["address"].toString());
    ui->linePassword->clear();
    ui->linePassword->setDisabled(true);
    ui->lineRepassword->clear();
    ui->lineRepassword->setDisabled(true);
    ui->pushSave->setEnabled(true);
    mId = data["id"].toInt();
    setWindowTitle(tr("Edit user"));
}

void UserAddDialog::enableSave()
{
    ui->pushSave->setEnabled(true);
}

void UserAddDialog::saveClicked()
{
    if(GuiUtil::anyEmpty(QList<QWidget*>() << ui->lineUsername << ui->linePassword << ui->lineRepassword << ui->lineName)) {
        QMessageBox::warning(this, tr("Error"), tr("Please fill all form"));
        return;
    }
    QVariantMap data;
    data["username"] = ui->lineUsername->text();
    data["password"] = QString(QCryptographicHash::hash(ui->linePassword->text().toUtf8(),QCryptographicHash::Md5).toHex());
    data["name"] = ui->lineName->text();
    data["phone"] = ui->linePhone->text();
    data["address"] = ui->textAddress->toPlainText();
    emit saveData(data, mId);
    ui->pushSave->setDisabled(true);
}
