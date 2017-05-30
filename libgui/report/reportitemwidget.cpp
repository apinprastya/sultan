#include "reportitemwidget.h"
#include "ui_normalwidget.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "tableview.h"
#include "headerwidget.h"
#include "global_constant.h"
#include "guiutil.h"

using namespace LibGUI;
using namespace LibG;

ReportItemWidget::ReportItemWidget(LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalWidget),
    mTableWidget(new TableWidget(this))
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->verticalLayout->addWidget(mTableWidget);
    ui->labelTitle->setText(tr("Item sales"));
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("barcode", tr("Barcode"));
    model->addColumn("name", tr("Name"));
    model->addColumn("stock", tr("Stock"));
    model->addColumn("count", tr("Sold"));
    model->addColumn("category", tr("Category"));
    model->addColumn("suplier", tr("Suplier"));
    model->addHeaderFilter("barcode", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("name", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->setTypeCommand(MSG_TYPE::SOLD_ITEM, MSG_COMMAND::SOLD_ITEM_REPORT);
    model->setTypeCommandOne(MSG_TYPE::SOLD_ITEM, MSG_COMMAND::GET);
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150 << 150 << 100 << 100 << 150 << 150);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    model->refresh();
}

ReportItemWidget::~ReportItemWidget()
{

}

void ReportItemWidget::messageReceived(LibG::Message */*msg*/)
{

}
