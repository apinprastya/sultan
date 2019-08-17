/*
 * preference.cpp
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
#include "preference.h"
#include "global_setting_const.h"
#include <QLocale>

//#define WIN_PORTABLE

using namespace LibG;

static Preference *sInstance = nullptr;

Preference::Preference():
#ifdef WIN_PORTABLE
    mSetting(new QSettings(QLatin1String("sultan.ini"), QSettings::IniFormat))
#else
    mSetting(new QSettings(QLatin1String("lekapin"), QLatin1String("sultan")))
#endif
{
}

Preference::~Preference()
{
    if(mSetting)
        delete mSetting;
    mSetting = nullptr;
}

Preference *Preference::instance()
{
    if(sInstance == nullptr)
        sInstance = new Preference();
    return sInstance;
}

void Preference::destroy()
{
    if(sInstance)
        delete sInstance;
    sInstance = nullptr;
}

void Preference::createInstance()
{
    instance();
    applyApplicationSetting();
}

void Preference::setValue(const QString &key, const QVariant &data)
{
    sInstance->mSetting->setValue(key, data);
}

int Preference::getInt(const QString &key, int def)
{
    return sInstance->mSetting->value(key, def).toInt();
}

QString Preference::getString(const QString &key, const QString &def)
{
    return sInstance->mSetting->value(key, def).toString();
}

bool Preference::getBool(const QString &key, bool def)
{
    return sInstance->mSetting->value(key, def).toBool();
}

double Preference::getDouble(const QString &key, double def)
{
    return sInstance->mSetting->value(key, def).toDouble();
}

void Preference::sync()
{
    sInstance->mSetting->sync();
}

void Preference::applyApplicationSetting()
{
    QLocale locale((QLocale::Language)Preference::getInt(SETTING::LOCALE_LANGUAGE, QLocale::Indonesian),
                   (QLocale::Country)Preference::getInt(SETTING::LOCALE_COUNTRY, QLocale::Indonesia));
    QLocale::setDefault(locale);
}

QString Preference::formatMoney(double val)
{
    return QLocale().toString(val, 'f', Preference::getInt(SETTING::LOCALE_DECIMAL));
}

QString Preference::formatMoney(int val)
{
    return QLocale().toString(val);
}

QString Preference::formatMoney(float val)
{
    return QLocale().toString(val, 'f', Preference::getInt(SETTING::LOCALE_DECIMAL));
}

QString Preference::formatFloat(float val)
{
    return QLocale().toString(val, 'f', 2);
}
