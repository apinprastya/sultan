#include "searchcustomerdialog.h"
#include "ui_searchcustomerdialog.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tableitem.h"
#include "global_constant.h"
#include "guiutil.h"
#include "db_constant.h"
#include "keyevent.h"
#include "headerwidget.h"
#include "tableitem.h"
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

SearchCustomerDialog::SearchCustomerDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchCustomerDialog)
{
    ui->setupUi(this);
    auto keyEvent = new KeyEvent(this);
    keyEvent->addConsumeKey(Qt::Key_Return);
    ui->table->installEventFilter(keyEvent);
    auto model = ui->table->getModel();
    model->setMessageBus(bus);
    model->addColumn("number", tr("ID"));
    model->addColumn("name", tr("Name"));
    model->addColumn("address", tr("Address"));
    model->addColumn("phone", tr("Phone"));
    model->addColumnMoney("reward", tr("Reward"));
    model->addColumnMoney("credit", tr("Credit"));
    model->setTypeCommand(MSG_TYPE::CUSTOMER, MSG_COMMAND::QUERY);
    ui->table->setupTable();
    connect(ui->lineName, SIGNAL(returnPressed()), SLOT(nameDone()));
    GuiUtil::setColumnWidth(ui->table->getTableView(), QList<int>() << 80 << 100 << 100 << 100 << 100 << 100);
    ui->table->getTableView()->horizontalHeader()->setStretchLastSection(true);
    ui->lineName->setFocus();
    ui->table->getTableView()->setUseStandardHeader(true);
    connect(ui->table->getModel(), SIGNAL(firstDataLoaded()), SLOT(dataLoaded()));
    connect(keyEvent, SIGNAL(keyPressed(QObject*,QKeyEvent*)), SLOT(returnPressed()));
    connect(ui->table->getTableView(), SIGNAL(doubleClicked(QModelIndex)), SLOT(doubleClicked(QModelIndex)));
}

SearchCustomerDialog::~SearchCustomerDialog()
{
    delete ui;
}

void SearchCustomerDialog::nameDone()
{
    const QString &name = ui->lineName->text();
    if(name.isEmpty()) return;
    ui->table->getModel()->setFilter("name", COMPARE::LIKE, name);
    ui->table->getModel()->refresh();
}

void SearchCustomerDialog::dataLoaded()
{
    if(ui->table->getModel()->rowCount(QModelIndex()) > 0) {
        ui->table->getTableView()->selectRow(0);
        ui->table->getTableView()->setFocus(Qt::TabFocusReason);
    }
}

void SearchCustomerDialog::returnPressed()
{
    const QModelIndex &index = ui->table->getTableView()->currentIndex();
    if(index.isValid()) {
        auto item = static_cast<TableItem*>(index.internalPointer());
        mSelectedData = item->data();
        mIsOk = true;
        close();
    }
}

void SearchCustomerDialog::doubleClicked(const QModelIndex &index)
{
    auto item = static_cast<TableItem*>(index.internalPointer());
    if(item) {
        mSelectedData = item->data();
        mIsOk = true;
        close();
    }
}
