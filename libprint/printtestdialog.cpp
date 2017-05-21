#include "printtestdialog.h"
#include "ui_printtestdialog.h"
#include "printer.h"
#include "escp.h"
#include "global_setting_const.h"
#include "global_constant.h"
#include "preference.h"
#include <QDebug>

using namespace LibPrint;
using namespace LibG;

PrintTestDialog::PrintTestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintTestDialog)
{
    ui->setupUi(this);
    QStringList printers = Printer::instance()->getPrintList();
    ui->comboPrint->addItems(printers);
    connect(ui->pushPrint, SIGNAL(clicked(bool)), SLOT(printClicked()));
    ui->comboType->addItem(QLatin1String("Device"), PRINT_TYPE::DEVICE);
    ui->comboType->addItem(QLatin1String("Spool"), PRINT_TYPE::SPOOL);
    connect(ui->comboType, SIGNAL(currentIndexChanged(int)), SLOT(updateEnable()));
    updateEnable();
}

PrintTestDialog::~PrintTestDialog()
{
    delete ui;
}

void PrintTestDialog::printClicked()
{
    Escp e(10, 10, 0);
    QString str;
    QString str2;
    QString str3;
    int counter = 1;
    while(counter <= ui->spin10->value()) {
        str.append(QString::number(counter % 10));
        counter++;
    }
    counter = 1;
    while(counter <= ui->spin12->value()) {
        str3.append(QString::number(counter % 10));
        counter++;
    }
    counter = 1;
    while(counter <= ui->spin15->value()) {
        str3.append(QString::number(counter % 10));
        counter++;
    }
    e.cpi10();
    e.leftText(str, true);
    e.newLine();
    e.leftText(QLatin1String("-----"));
    e.newLine();
    e.cpi12();
    e.leftText(str2, true);
    e.newLine();
    e.leftText(QLatin1String("-----"));
    e.newLine();
    e.cpi15();
    e.leftText(str3, true);
    e.newLine();
    e.leftText(QLatin1String("-----"));
    e.newLine(3);
    int type = ui->comboType->currentData().toInt();
    if(type == PRINT_TYPE::DEVICE)
        Printer::instance()->print(ui->lineDevice->text(), e.data(), ui->comboType->currentData().toInt());
    else
        Printer::instance()->print(ui->comboPrint->currentText(), e.data(), ui->comboType->currentData().toInt());
}

void PrintTestDialog::updateEnable()
{
    if(ui->comboType->currentIndex() == 0) {
        ui->comboPrint->setEnabled(false);
        ui->lineDevice->setEnabled(true);
    } else {
        ui->comboPrint->setEnabled(true);
        ui->lineDevice->setEnabled(false);
    }
}
