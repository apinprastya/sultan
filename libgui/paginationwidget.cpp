#include "paginationwidget.h"
#include "ui_paginationwidget.h"
#include <QSpinBox>

using namespace LibGUI;

PaginationWidget::PaginationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PaginationWidget)
{
    ui->setupUi(this);
    ui->comboPerPage->addItem(tr("10/page"), 10);
    ui->comboPerPage->addItem(tr("25/page"), 25);
    ui->comboPerPage->addItem(tr("50/page"), 50);
    ui->comboPerPage->addItem(tr("100/page"), 100);
    connect(ui->spinPage, QOverload<int>::of(&QSpinBox::valueChanged), this, &PaginationWidget::spinChanged);
    connect(ui->comboPerPage, SIGNAL(currentIndexChanged(int)), SLOT(comboIndexChanged()));
}

PaginationWidget::~PaginationWidget()
{
    delete ui;
}

void PaginationWidget::setCurrentPerPage(int index, bool blockSignal)
{
    ui->comboPerPage->blockSignals(blockSignal);
    ui->comboPerPage->setCurrentIndex(index);
    ui->comboPerPage->blockSignals(false);
}

void PaginationWidget::setMaxPage(int value)
{
    ui->spinPage->setMaximum(value);
    ui->labelMax->setText(QString::number(value));
}

void PaginationWidget::spinChanged(int value)
{
    emit pageChanged(value - 1);
}

void PaginationWidget::comboIndexChanged()
{
    setCurrentPage(1);
    emit perPageChanged(ui->comboPerPage->currentData().toInt());
}

void PaginationWidget::setCurrentPage(int value)
{
    ui->spinPage->blockSignals(true);
    ui->spinPage->setValue(value);
    ui->spinPage->blockSignals(false);
}
