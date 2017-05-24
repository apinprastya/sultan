#include "searchitemdialog.h"
#include "ui_searchitemdialog.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tableitem.h"
#include "global_constant.h"
#include "guiutil.h"
#include "db_constant.h"
#include "keyevent.h"

using namespace LibGUI;
using namespace LibG;

SearchItemDialog::SearchItemDialog(MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchItemDialog),
    mTableWidget(new TableWidget(this, true))
{
    ui->setupUi(this);
    auto keyEvent = new KeyEvent(this);
    keyEvent->addConsumeKey(Qt::Key_Return);
    mTableWidget->installEventFilter(keyEvent);
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("barcode", tr("Barcode"));
    model->addColumn("name", tr("Name"));
    model->addColumnMoney("sell_price", tr("Price"));
    model->addColumnMoney("stock", tr("Stock"));
    model->setTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::QUERY);
    mTableWidget->setupTable();
    ui->verticalLayout->addWidget(mTableWidget);
    connect(ui->lineName, SIGNAL(returnPressed()), SLOT(nameDone()));
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 200 << 200 << 100 << 80);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    ui->lineName->setFocus();
    connect(mTableWidget->getModel(), SIGNAL(firstDataLoaded()), SLOT(dataLoaded()));
    connect(keyEvent, SIGNAL(keyPressed(QObject*,QKeyEvent*)), SLOT(returnPressed()));
}

SearchItemDialog::~SearchItemDialog()
{
    delete ui;
}

void SearchItemDialog::nameDone()
{
    const QString &name = ui->lineName->text();
    if(name.isEmpty()) return;
    mTableWidget->getModel()->setFilter("name", COMPARE::LIKE, name);
    mTableWidget->getModel()->refresh();
}

void SearchItemDialog::dataLoaded()
{
   if(mTableWidget->getModel()->rowCount(QModelIndex()) > 0) {
       mTableWidget->getTableView()->selectRow(0);
       mTableWidget->setFocus(Qt::TabFocusReason);
   }
}

void SearchItemDialog::returnPressed()
{
    const QModelIndex &index = mTableWidget->getTableView()->currentIndex();
    if(index.isValid()) {
        auto item = static_cast<TableItem*>(index.internalPointer());
        mSelectedBarcode = item->data("barcode").toString();
        close();
    }
}
