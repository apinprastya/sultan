/*
 * browserdialog.cpp
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
#include "browserdialog.h"
#include "ui_browserdialog.h"
#include <QLineEdit>
#include <QProgressBar>
#include <QUrl>
#ifdef USE_EMBED_BROWSER
#ifndef USE_WEBENGINE
#include <QWebView>
#else
#include <QWebEngineView>
#endif
#endif

using namespace LibGUI;

BrowserDialog::BrowserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BrowserDialog),
    mLineAddress(new QLineEdit(this))
#ifdef USE_EMBED_BROWSER
#ifndef USE_WEBENGINE
    ,mWebView(new QWebView(this))
#else
    ,mWebView(new QWebEngineView(this))
#endif
#endif
{
    ui->setupUi(this);
    ui->verticalLayout->addWidget(mLineAddress);
#ifdef USE_EMBED_BROWSER
    ui->verticalLayout->addWidget(mWebView);
#endif
    mLineAddress->setReadOnly(true);
}

BrowserDialog::~BrowserDialog()
{
    delete ui;
}

void BrowserDialog::setUrl(const QUrl &url)
{
#ifdef USE_EMBED_BROWSER
    mWebView->load(url);
#endif
    mLineAddress->setText(url.toString());
}
