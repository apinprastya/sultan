/*
 * serveraction.h
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
#ifndef SERVERACTION_H
#define SERVERACTION_H

#include "server_global.h"
#include "message.h"
#include <QMap>
#include <functional>

namespace LibDB {
class Db;
}

namespace LibServer {

class SERVERSHARED_EXPORT ServerAction
{
public:
    enum Flag {
        AFTER_INSERT        = 0x1,
        AFTER_UPDATE        = 0x2,
        HAS_UPDATE_FIELD    = (1 << 16),
    };

    ServerAction(const QString &tableName, const QString idfield);
    virtual ~ServerAction();
    LibG::Message exec(LibG::Message *msg);

protected:
    int mFlag = 0;
    LibDB::Db *mDb;
    QString mTableName;
    QString mIdField;
    QMap<int, std::function<LibG::Message(LibG::Message*)> > mFunctionMap;

    virtual LibG::Message insert(LibG::Message *msg);
    virtual LibG::Message update(LibG::Message *msg);
    virtual LibG::Message del(LibG::Message *msg);
    virtual LibG::Message get(LibG::Message *msg);
    virtual LibG::Message query(LibG::Message *msg);

    void setStart(LibG::Message *msg, LibG::Message *src);
    virtual QMap<QString, QString> fieldMap() const;
    virtual void selectAndJoin() {}
    virtual void afterInsert(const QVariantMap &/*data*/) {}
    virtual void afterUpdate(const QVariantMap &/*oldData*/, const QVariantMap &/*newData*/) {}

    bool hasFlag(int flag);
};

}
#endif // SERVERACTION_H
