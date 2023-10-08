/*
 * importexportdatabasedialog.h
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
#ifndef IMPORTEXPORTDATABASEDIALOG_H
#define IMPORTEXPORTDATABASEDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class ImportExportDatabaseDialog;
}

class QProgressDialog;

namespace LibGUI {

class BrowserDialog;

class ImportExportDatabaseDialog : public QDialog, public LibG::MessageHandler {
    Q_OBJECT

  public:
    ImportExportDatabaseDialog(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~ImportExportDatabaseDialog() override;

  protected:
    void messageReceived(LibG::Message *msg) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

  private:
    Ui::ImportExportDatabaseDialog *ui;
    bool mIsExport = false;
    void uploadFile(const QByteArray &data);

  private slots:
    void exportFile();
    void importFile();
};

} // namespace LibGUI
#endif // IMPORTEXPORTDATABASEDIALOG_H
