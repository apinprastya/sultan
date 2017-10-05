#include "restoreitemdialog.h"
#include "ui_restoreitemdialog.h"

using namespace LibGUI;

RestoreItemDialog::RestoreItemDialog(const QVariantMap &data, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RestoreItemDialog)
{
    ui->setupUi(this);
    ui->labelBarcode->setText(data["barcode"].toString());
    ui->labelName->setText(data["name"].toString());
    ui->labelUnit->setText(data["unit"].toString());
    ui->labelCategory->setText(data["category"].toString());
    ui->labelSuplier->setText(data["suplier"].toString());
    connect(ui->pushRestore, SIGNAL(clicked(bool)), SLOT(restoreClicked()));
}

RestoreItemDialog::~RestoreItemDialog()
{
    delete ui;
}

void RestoreItemDialog::restoreClicked()
{
    mIsOk = true;
    close();
}
