/*
 * browserdialog.h
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
#ifndef BROWSERDIALOG_H
#define BROWSERDIALOG_H

#include <QDialog>

class QLineEdit;

#ifdef USE_EMBED_BROWSER
#ifndef USE_WEBENGINE
class QWebView;
#else
class QWebEngineView;
#endif
#endif

namespace Ui {
class BrowserDialog;
}

namespace LibGUI {

class BrowserDialog : public QDialog {
    Q_OBJECT

  public:
    BrowserDialog(QWidget *parent = nullptr);
    ~BrowserDialog();
    void setUrl(const QUrl &url);

  private:
    Ui::BrowserDialog *ui;
    QLineEdit *mLineAddress;
#ifdef USE_EMBED_BROWSER
#ifndef USE_WEBENGINE
    QWebView *mWebView;
#else
    QWebEngineView *mWebView;
#endif
#endif
};

} // namespace LibGUI
#endif // BROWSERDIALOG_H
