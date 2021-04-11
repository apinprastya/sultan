/*
 * stockcarddialog.cpp
 * Copyright 2017 - ~, Apin <apin.klas@gmail.com>
 *
 * This file is part of Sultan.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "stockcarddialog.h"
#include "db_constant.h"
#include "dbutil.h"
#include "global_constant.h"
#include "guiutil.h"
#include "headerwidget.h"
#include "tableitem.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tablewidget.h"
#include "ui_stockcarddialog.h"

using namespace LibGUI;
using namespace LibG;

StockCardDialog::StockCardDialog(const QString &barcode, LibG::MessageBus *bus, QWidget *parent)
    : QDialog(parent), ui(new Ui::StockCardDialog) {
    ui->setupUi(this);
    setWindowTitle(tr("Stock Card : %1").arg(barcode));
    auto model = ui->table->getModel();
    model->setMessageBus(bus);
    model->addColumn("created_at", tr("Date time"), Qt::AlignLeft, [](TableItem *item, const QString &key) {
        return LibDB::DBUtil::sqlDateToDateTime(item->data(key).toString()).toString("dd-MM-yyyy hh:mm");
    });
    model->addColumn("type", tr("Type"), Qt::AlignLeft, [](TableItem *item, const QString &key) {
        switch (item->data(key).toInt()) {
        case STOCK_CARD_TYPE::INITIAL_STOCK:
            return tr("Initial");
        case STOCK_CARD_TYPE::PURCHASE:
            return tr("Purchase");
        case STOCK_CARD_TYPE::SOLD:
            return tr("Sold");
        case STOCK_CARD_TYPE::CHECKSTOCK:
            return tr("Checkstock");
        case STOCK_CARD_TYPE::PURCHASE_RETURN:
            return tr("Purchase return");
        case STOCK_CARD_TYPE::SOLD_RETURN:
            return tr("Sold return");
        }
        return QString();
    });
    model->addColumn("number", tr("Number"));
    model->addColumn("count", tr("Count"), Qt::AlignRight);
    model->addColumn("stock", tr("Stock"), Qt::AlignRight);
    model->addHeaderFilter("type", HeaderFilter{HeaderWidget::Combo, TableModel::FilterEQ, QVariant()});
    model->setTypeCommand(MSG_TYPE::STOCKCARD, MSG_COMMAND::QUERY);
    model->setSort("id DESC");
    model->setFilter("barcode", COMPARE::EQUAL, barcode);
    ui->table->setupTable();
    GuiUtil::setColumnWidth(ui->table->getTableView(), QList<int>() << 120 << 150 << 250 << 100 << 100);
    ui->table->getTableView()->horizontalHeader()->setStretchLastSection(true);
    model->refresh();
}

StockCardDialog::~StockCardDialog() { delete ui; }

void StockCardDialog::showEvent(QShowEvent *e) {
    QDialog::showEvent(e);
    auto combo = ui->table->getTableView()->getHeaderWidget(ui->table->getModel()->getIndex("type"))->getComboBox();
    if (combo->count() == 0) {
        combo->blockSignals(true);
        combo->clear();
        combo->addItem(tr("All"), -1);
        combo->addItem(tr("Initial"), STOCK_CARD_TYPE::INITIAL_STOCK);
        combo->addItem(tr("Purchase"), STOCK_CARD_TYPE::PURCHASE);
        combo->addItem(tr("Sold"), STOCK_CARD_TYPE::SOLD);
        combo->addItem(tr("Checkstock"), STOCK_CARD_TYPE::CHECKSTOCK);
        combo->addItem(tr("Purchase return"), STOCK_CARD_TYPE::PURCHASE_RETURN);
        combo->addItem(tr("Sold return"), STOCK_CARD_TYPE::SOLD_RETURN);
        combo->blockSignals(false);
    }
}
