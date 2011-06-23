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
#include <boost/assert.hpp>
#include <boost/thread.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include "cetty/channel/Channel.h"

#include "cetty/channel/socket/asio/AsioDatagramChannel.h"
#include "cetty/channel/socket/asio/AsioDatagramChannelFactory.h"
#include "cetty/channel/socket/asio/AsioDatagramPipelineSink.h"

#include "cetty/channel/socket/asio/AsioIpAddressImplFactory.h"
#include "cetty/channel/socket/asio/AsioSocketAddressImplFactory.h"
#include "cetty/util/internal/asio/AsioDeadlineTimerFactory.h"

namespace cetty { namespace channel { namespace socket { namespace asio {

using namespace cetty::channel;
using namespace cetty::util::internal::asio;

AsioDatagramChannelFactory::AsioDatagramChannelFactory(int ioThreadCount /*= 1*/)
    : started(false),
      ioThreadCount(ioThreadCount),
      ipProtocol(IpAddress::IPv4),
      ioServicePool(ioThreadCount) {
    this->sink = new AsioDatagramPipelineSink(ioServicePool);

    timerFactory = TimerFactoryPtr(new AsioDatagramDeadlineTimerFactory(ioServicePool));
    TimerFactory::setFactory(timerFactory);

    createSocketAddressImplFactory();
}

AsioDatagramChannelFactory::~AsioDatagramChannelFactory() {
    if (sink) {
        delete sink;
    }
    destorySocketAddressImplFactory();
}

Channel* AsioDatagramChannelFactory::newChannel(cetty::channel::ChannelPipeline* pipeline) {
    AsioDatagramChannel* channel;

    try {
        channel = new AsioDatagramChannel(
            this, pipeline, sink, ioServicePool.getIOService(), ioThreadCount, ipProtocol);
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

    return (Channel*)channel;
}

void AsioDatagramChannelFactory::releaseExternalResources() {

}

void AsioDatagramChannelFactory::start() {
    if (started) return;
    ioServicePool.run();
    started = true;
}

void AsioDatagramChannelFactory::createSocketAddressImplFactory() {
    socketAddressFactory = new AsioUdpSocketAddressImplFactory(ioServicePool.getIOService());
    ipAddressFactory = new AsioIpAddressImplFactory();
    BOOST_ASSERT(socketAddressFactory);
    BOOST_ASSERT(ipAddressFactory);

    SocketAddress::setFacotry(socketAddressFactory);
    IpAddress::setFactory(ipAddressFactory);
}

void AsioDatagramChannelFactory::destorySocketAddressImplFactory() {
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
