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
#include "cetty/channel/socket/asio/AsioServerSocketChannelFactory.h"

#include "cetty/channel/IpAddress.h"
#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/socket/asio/AsioServerSocketChannel.h"
#include "cetty/channel/socket/asio/AsioServerSocketPipelineSink.h"
#include "cetty/channel/socket/asio/AsioSocketAddressImplFactory.h"
#include "cetty/channel/socket/asio/AsioIpAddressImplFactory.h"
#include "cetty/channel/socket/asio/AsioServicePool.h"
#include "cetty/channel/socket/asio/AsioSocketChannel.h"
#include "cetty/util/internal/asio/AsioDeadlineTimerFactory.h"

namespace cetty { namespace channel { namespace socket { namespace asio { 

using namespace cetty::channel;
using namespace cetty::util::internal::asio;

AsioServerSocketChannelFactory::AsioServerSocketChannelFactory(int ioThreadCount)
    : ipProtocol(IpAddress::IPv4),
      ioServicePool(ioThreadCount),
      acceptor(ioServicePool.getIOService()) {

    this->sink = new AsioServerSocketPipelineSink(ioServicePool, acceptor);

    timerFactory = TimerFactoryPtr(new AsioSocketDeadlineTimerFactory(ioServicePool));
    TimerFactory::setFactory(timerFactory);

    socketAddressFactory = new AsioTcpSocketAddressImplFactory(acceptor.io_service());
    SocketAddress::setFacotry(socketAddressFactory);

    ipAddressFactory = new AsioIpAddressImplFactory();
    IpAddress::setFactory(ipAddressFactory);
}

AsioServerSocketChannelFactory::~AsioServerSocketChannelFactory() {
    if (this->sink) {
        delete sink;
    }

    if (socketAddressFactory) {
        delete socketAddressFactory;
        socketAddressFactory = NULL;
    }

    if (ipAddressFactory) {
        delete ipAddressFactory;
        ipAddressFactory = NULL;
    }
}

Channel* AsioServerSocketChannelFactory::newChannel(cetty::channel::ChannelPipeline* pipeline) {
    AsioServerSocketChannel* channel;
    try {
        channel = new AsioServerSocketChannel(acceptor.io_service(),
                                              acceptor,
                                              this,
                                              pipeline,
                                              sink,
                                              ipProtocol);
        ioServicePool.run();
    }
    catch (const ChannelException& e) {
        e.rethrow();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException("boost asio run has exception", e.code().value());
    }
    catch (...) {
        throw ChannelException("may be memory allocation error, or others");
    }

    channels.push_back(channel);
    return (Channel*)channel;
}

void AsioServerSocketChannelFactory::releaseExternalResources() {
    ioServicePool.stop();
    ioServicePool.waitForExit();

    std::vector<AsioServerSocketChannel*>::iterator itr;
    for (itr = channels.begin(); itr != channels.end(); ++itr) {
        delete *itr;
    }
    channels.clear();
}

}}}}