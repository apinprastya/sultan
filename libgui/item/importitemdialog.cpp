#include "importitemdialog.h"
#include "ui_importitemdialog.h"

using namespace LibGUI;

ImportItemDialog::ImportItemDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ImportItemDialog) {
    ui->setupUi(this);
}

ImportItemDialog::~ImportItemDialog() { delete ui; }

void LibGUI::ImportItemDialog::on_pushImport_clicked() {
    mIsChecked = ui->groupBox->isChecked();
    accept();
}
