#include "categotyadddialog.h"
#include "ui_categoryadddialog.h"
#include "guiutil.h"
#include <QMessageBox>
#include <QDebug>

using namespace LibGUI;

CategoryAddDialog::CategoryAddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CategoryAddDialog)
{
    ui->setupUi(this);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

CategoryAddDialog::~CategoryAddDialog()
{
    delete ui;
}

void CategoryAddDialog::reset()
{
    ui->lineCode->clear();
    ui->lineName->clear();
    ui->comboParent->setFocus(Qt::TabFocusReason);
    ui->comboParent->setDisabled(false);
    ui->pushSave->setEnabled(true);
    mId = -1;
}

void CategoryAddDialog::fill(int id, int parent, const QString &name, const QString &code)
{
    for(int i = 0; i < ui->comboParent->count(); i++) {
        if(parent == ui->comboParent->itemData(i).toInt()) {
            ui->comboParent->setCurrentIndex(i);
            break;
        }
    }
    ui->lineName->setText(name);
    ui->lineCode->setText(code);
    ui->lineName->setFocus(Qt::TabFocusReason);
    ui->comboParent->setDisabled(true);
    ui->pushSave->setEnabled(true);
    mId = id;
}

void CategoryAddDialog::enableSaveButton(bool enable)
{
    ui->pushSave->setEnabled(enable);
}

QComboBox *CategoryAddDialog::getComboParent()
{
    return ui->comboParent;
}

void CategoryAddDialog::saveClicked()
{
    if(GuiUtil::anyEmpty(QList<QWidget*>() << ui->lineName << ui->lineCode)) {
        QMessageBox::warning(this, tr("Error"), tr("Please fill all form"));
        return;
    }
    QVariantMap data;
    data.insert("name", ui->lineName->text());
    data.insert("code", ui->lineCode->text());
    if(ui->comboParent->isEnabled())
        data.insert("parent_id", ui->comboParent->currentData().toInt());
    ui->pushSave->setEnabled(false);
    emit saveRequest(data, mId);
}
