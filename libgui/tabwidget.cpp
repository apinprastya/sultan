/*
 * tabview.cpp
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
#include "tabwidget.h"
#include "tabcloseablewidget.h"
#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QShortcut>
#include <QVBoxLayout>

using namespace LibGUI;

TabWidget::TabWidget(QWidget *parent) : TabWidget(Root, parent) {}

TabWidget::TabWidget(int type, QWidget *parent) : QTabWidget(parent), mType(type) {
    if (mType == Root) {
        auto lay = new QVBoxLayout(this);
        mLabel = new QLabel();
        QPixmap px(QLatin1String(":/images/icon_512.png"));
        mLabel->setPixmap(px);
        mLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        lay->addWidget(mLabel);
        setLayout(lay);
    } else if (mType == Purchase) {
        setTabPosition(QTabWidget::West);
        new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageDown), this, SLOT(nextTab()));
        new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageUp), this, SLOT(prevTab()));
    } else {
        setTabPosition(QTabWidget::West);
        new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_T), this, SLOT(newTab()));
        new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageDown), this, SLOT(nextTab()));
        new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageUp), this, SLOT(prevTab()));
    }
    setTabsClosable(true);
    connect(this, SIGNAL(tabCloseRequested(int)), SLOT(tbnRemoveTab(int)));
    setTabBarAutoHide(true);
}

void TabWidget::tbnAddTab(QWidget *widget, const QString &name) {
    if (mType == Root && mLabel->isVisible())
        mLabel->setVisible(false);
    addTab(widget, name);
    setCurrentIndex(indexOf(widget));
}

void TabWidget::tbnAddTab(QWidget *widget, const QString &name, const QString &iconName) {
    if (mType == Root && mLabel->isVisible())
        mLabel->setVisible(false);
    addTab(widget, QIcon(iconName), name);
    setCurrentIndex(indexOf(widget));
}

void TabWidget::tbnRemoveTab(int index) {
    if (mType != Root && index == 0)
        return;
    if (mType == Root) {
        TabWidget *w = dynamic_cast<TabWidget *>(widget(index));
        if (w != nullptr && w->mType == Cashier) {
            for (int i = w->count() - 1; i >= 0; i--) {
                auto tc = dynamic_cast<TabCloseableWidget *>(w->widget(i));
                if (tc != nullptr) {
                    w->setCurrentIndex(i);
                    qApp->processEvents();
                    if (!tc->requestClose()) {
                        return;
                    }
                    w->tbnRemoveTab(i);
                }
            }
        }
    } else if (mType == Cashier) {
        auto tc = dynamic_cast<TabCloseableWidget *>(widget(index));
        if (tc != nullptr && !tc->requestClose())
            return;
    }
    widget(index)->deleteLater();
    removeTab(index);
    if (count() == 0 && mType == Root)
        mLabel->setVisible(true);
}

void TabWidget::tbnRemoveTab() {
    auto w = static_cast<QWidget *>(QObject::sender());
    for (int i = 0; i < count(); i++)
        if (widget(i) == w)
            tbnRemoveTab(i);
}

bool TabWidget::isTabAvailable(std::function<bool(QWidget *)> func) {
    for (int i = 0; i < count(); i++) {
        bool ok = func(widget(i));
        if (ok) {
            setCurrentIndex(indexOf(widget(i)));
            return true;
        }
    }
    return false;
}

void TabWidget::closeAllTabAndFree() {
    for (int i = count() - 1; i >= 0; i--) {
        auto w = widget(i);
        w->deleteLater();
        tbnRemoveTab(i);
    }
}

void TabWidget::newTab() {
    if (mNewWidgetFunc != nullptr) {
        addTab(mNewWidgetFunc(), QString("#%1").arg(mCounter++, 3, 10, QChar('0')));
        setCurrentIndex(count() - 1);
    }
}

void TabWidget::nextTab() {
    if (count() == 1)
        return;
    int cur = currentIndex() + 1;
    if (cur >= count())
        cur = 0;
    setCurrentIndex(cur);
}

void TabWidget::prevTab() {
    if (count() == 1)
        return;
    int cur = currentIndex() - 1;
    if (cur < 0)
        cur = count() - 1;
    setCurrentIndex(cur);
}
