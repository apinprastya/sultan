/*
 * message.cpp
 * Copyright 2017 - ~, Apin <apin.klas@gmail.com>
 *
 * This file is part of Turbin.
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
#include "message.h"
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

using namespace LibG;

Message::Message(int type, int status)
{
    setType(type);
    setStatus(status);
}

Message::Message(const QByteArray &ba)
{
    fromByteArray(ba);
}

void Message::setType(int type)
{
    mFlag &= ~0xFFFF;
    mFlag |= (type & 0xFFFF);
}

void Message::setStatus(int status)
{
    mFlag &= ~0xF00000;
    mFlag |= ((status << 16) & 0xF);
}

QJsonObject Message::toJsonObject()
{
    QJsonObject root;
    root.insert(QLatin1String("_u"), QJsonValue(mUniqueId));
    root.insert(QLatin1String("_f"), QJsonValue(mFlag));
    root.insert(QLatin1String("_fr"), QJsonValue(mFlagReply));
    root.insert(QLatin1String("_d"), QJsonValue(QJsonObject::fromVariantMap(mData)));
    return root;
}

QString Message::toJsonString()
{
    const QJsonObject &root = toJsonObject();
    QJsonDocument doc(root);
    return QLatin1String(doc.toJson(QJsonDocument::Compact));
}

QByteArray Message::toByteArray()
{
    const QJsonObject &root = toJsonObject();
    QJsonDocument doc(root);
    return qCompress(doc.toBinaryData());
}

void Message::fromByteArray(const QByteArray &ba)
{
    QJsonDocument doc = QJsonDocument::fromBinaryData(qUncompress(ba));
    fromJsonDoc(doc);
}

void Message::fromJsonDoc(const QJsonDocument &jsonDoc)
{
    QJsonObject obj = jsonDoc.object();
    mFlag = obj.value(QLatin1String("_f")).toInt();
    mFlagReply = obj.value(QLatin1String("_fr")).toInt();
    mData = obj.value(QLatin1String("_d")).toObject().toVariantMap();
    mUniqueId = obj.value(QLatin1String("_u")).toInt();
}
