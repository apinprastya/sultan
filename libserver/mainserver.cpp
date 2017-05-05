/*
 * libserver.cpp
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
#include "mainserver.h"
#include "futurewatcher.h"
#include "router.h"
#include "message.h"
#include <QtConcurrent>

using namespace LibG;
using namespace LibServer;

MainServer::MainServer(QObject *parent) :
    QObject(parent),
    mRouter(new Router)
{

}

MainServer::~MainServer()
{
    if(mRouter) delete mRouter;
}

void MainServer::messageReceived(LibG::Message *msg)
{
    auto watcher = new FutureWatcher();
    //connect(watcher, SIGNAL(messageReceived(Message*)), SIGNAL(messageReady(Message*)));
    watcher->setFuture(QtConcurrent::run(mRouter, &Router::handler, *msg));
}
