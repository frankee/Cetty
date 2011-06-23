/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include "cetty/channel/socket/asio/AsioServicePool.h"
#include "cetty/channel/socket/asio/AsioIpAddressImplFactory.h"
#include "cetty/channel/socket/asio/AsioSocketAddressImplFactory.h"
#include "cetty/channel/socket/asio/AsioClientSocketChannel.h"
#include "cetty/channel/socket/asio/AsioClientSocketPipelineSink.h"
#include "cetty/channel/socket/asio/AsioClientSocketChannelFactory.h"
#include "cetty/util/internal/asio/AsioDeadlineTimerFactory.h"
#include "cetty/util/Exception.h"

namespace cetty { namespace channel { namespace socket { namespace asio {

using namespace cetty::util;
using namespace cetty::util::internal::asio;

AsioClientSocketChannelFactory::AsioClientSocketChannelFactory(int ioThreadCount)
    : ipProtocol(IpAddress::IPv4), ioServicePool(ioThreadCount) {
    sink = new AsioClientSocketPipelineSink(ioServicePool);

    timerFactory = TimerFactoryPtr(new AsioSocketDeadlineTimerFactory(ioServicePool));
    TimerFactory::setFactory(timerFactory);

    createSocketAddressImplFactory();
}

AsioClientSocketChannelFactory::~AsioClientSocketChannelFactory() {
    destorySocketAddressImplFactory();

    if (NULL != sink) {
        delete sink;
    }
}

Channel* AsioClientSocketChannelFactory::newChannel(
                    cetty::channel::ChannelPipeline* pipeline) {
    AsioServicePool::IOService& ioService = ioServicePool.getIOService();
    AsioClientSocketChannel* client =
        new AsioClientSocketChannel(this,
                                    pipeline,
                                    sink,
                                    ioService,
                                    ioServicePool.getThreadId(ioService.index()));

    clientChannels.push_back(client);
    return client;
}

void AsioClientSocketChannelFactory::releaseExternalResources() {
    ioServicePool.stop();
    ioServicePool.waitForExit();

    std::vector<Channel*>::iterator itr;
    for (itr = clientChannels.begin(); itr != clientChannels.end(); ++itr) {
        delete *itr;
    }
    clientChannels.clear();
}

void AsioClientSocketChannelFactory::start() {
    ioServicePool.run();
}

void AsioClientSocketChannelFactory::createSocketAddressImplFactory() {
    socketAddressFactory = new AsioTcpSocketAddressImplFactory(ioServicePool.getIOService());
    ipAddressFactory = new AsioIpAddressImplFactory();

    SocketAddress::setFacotry(socketAddressFactory);
    IpAddress::setFactory(ipAddressFactory);
}

void AsioClientSocketChannelFactory::destorySocketAddressImplFactory() {
    if (socketAddressFactory) {
        delete socketAddressFactory;
        socketAddressFactory = NULL;
    }

    if (ipAddressFactory) {
        delete ipAddressFactory;
        ipAddressFactory = NULL;
    }
}

}}}}
