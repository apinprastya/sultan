/*
 * logocached.cpp
 * Copyright 2023, Apin <apin.klas@gmail.com>
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

#include "logocached.h"
#include "global_setting_const.h"
#include "preference.h"
#include <QDebug>
#include <QFile>

namespace LibGUI {

LogoCached *LogoCached::instance = nullptr;

void LogoCached::createInstance(QObject *parent) {
    instance = new LogoCached(parent);
    instance->load();
}

QPixmap LogoCached::logo32() { return instance->mLogo32; }

QPixmap LogoCached::logo64() { return instance->mLogo64; }

QPixmap LogoCached::logo128() { return instance->mLogo128; }

QPixmap LogoCached::logo256() { return instance->mLogo256; }

QPixmap LogoCached::logo512() { return instance->mLogo512; }

LogoCached::LogoCached(QObject *parent) : QObject(parent) {}

void LogoCached::load() {
    const QPixmap &mainPixmap = getMainPixmap();
    mLogo512 = mainPixmap.scaled(512, 512);
    mLogo256 = mainPixmap.scaled(256, 256);
    mLogo128 = mainPixmap.scaled(128, 128);
    mLogo64 = mainPixmap.scaled(64, 64);
    mLogo32 = mainPixmap.scaled(32, 32);
}

QPixmap LogoCached::getMainPixmap() {
    const QString &logoPath = LibG::Preference::getString(LibG::SETTING::LOGO);
    if (QFile::exists(logoPath)) {
        QPixmap px;
        if (px.load(logoPath)) {
            return px;
        }
    }
    return QPixmap(QLatin1String(":/images/icon_512.png"));
}

} // namespace LibGUI
