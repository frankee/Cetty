#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVERSOCKETCHANNEL_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVERSOCKETCHANNEL_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/ChannelConfig.h"
#include "cetty/channel/socket/ServerSocketChannel.h"
#include "cetty/channel/socket/asio/DefaultAsioServerSocketChannelConfig.h"
#include "cetty/channel/socket/asio/AsioSocketAddressImpl.h"

namespace cetty { namespace logging {
class InternalLogger;
}}

namespace cetty { namespace channel { namespace socket { namespace asio {

using namespace cetty::logging;
using namespace cetty::channel;
using namespace cetty::channel::socket;

// only response to bind port, open and close.
class AsioServerSocketChannel : public cetty::channel::socket::ServerSocketChannel {
public:
    AsioServerSocketChannel(
            boost::asio::io_service& ioService,
            boost::asio::ip::tcp::acceptor& acceptor,
            ChannelFactory* factory,
            cetty::channel::ChannelPipeline* pipeline,
            cetty::channel::ChannelSink* sink,
            int ipProtocol)
            : ServerSocketChannel(factory, pipeline, sink),
              ioService(ioService),
              acceptor(acceptor),
              config(acceptor) {
        try {
            if (ipProtocol == IpAddress::IPv4) {
                acceptor.open(boost::asio::ip::tcp::v4());
            }
            else {
                acceptor.open(boost::asio::ip::tcp::v6());
            }
        }
        catch (const boost::system::system_error& e) {
            throw ChannelException("Failed to open a server socket.", e.code().value());
        }

        Channels::fireChannelOpen(*this);
    }

    virtual ~AsioServerSocketChannel() {
    }

    boost::asio::ip::tcp::acceptor& getAcceptor() {
        return acceptor;
    }

    cetty::channel::ChannelConfig& getConfig() { return config; }
    const cetty::channel::ChannelConfig& getConfig() const {
        return config;
    }

    const SocketAddress& getLocalAddress() const {
        if (localAddress != SocketAddress::NULL_ADDRESS) {
            return localAddress;
        }

        boost::system::error_code ec;
        boost::asio::ip::tcp::endpoint endpoint = acceptor.local_endpoint(ec);

        if (ec) {
            return SocketAddress::NULL_ADDRESS;
        }

        localAddress = SocketAddress(
            SocketAddress::SmartPointer(new AsioTcpSocketAddressImpl(ioService, endpoint)));

        return localAddress;
    }

    const SocketAddress& getRemoteAddress() const {
        return SocketAddress::NULL_ADDRESS;
    }

    bool isBound() const {
        return isOpen() && acceptor.is_open();
    }

    bool setClosed() {
        return AbstractChannel::setClosed();
    }

private:
    static InternalLogger* logger;

    boost::asio::io_service& ioService;
    boost::asio::ip::tcp::acceptor& acceptor;

    DefaultAsioServerSocketChannelConfig config;
    mutable SocketAddress localAddress;
};

}}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVERSOCKETCHANNEL_H)
