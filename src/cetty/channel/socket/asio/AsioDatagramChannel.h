#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIODATAGRAMCHANNEL_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIODATAGRAMCHANNEL_H

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

#include <deque>
#include <boost/asio.hpp>

#include "cetty/channel/IpAddress.h"
#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/NetworkInterface.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/channel/socket/DatagramChannel.h"
#include "cetty/channel/socket/asio/AsioServicePool.h"
#include "cetty/channel/socket/asio/DefaultAsioDatagramChannelConfig.h"
#include "cetty/channel/socket/asio/AsioWriteRequestQueue.h"
#include "cetty/channel/socket/asio/handler_allocator.hpp"

#include "cetty/buffer/ChannelBuffer.h"

namespace cetty { namespace channel {
class ChannelSink;
class ChannelPipeline;
class ChannelFactory;
class ChannelConfig;
class ChannelFuture;
}}

namespace cetty { namespace channel { namespace socket { namespace asio {

using namespace cetty::buffer;
using namespace cetty::channel;

class AsioDatagramChannel;
class DefaultAsioDatagramChannelConfig;

/**
 * Provides an ASIO based {@link org.jboss.netty.channel.socket.DatagramChannel}.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author Daniel Bevenius (dbevenius@jboss.com)
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class AsioDatagramChannel : public cetty::channel::socket::DatagramChannel {
public:
    typedef boost::asio::ip::udp::endpoint EndpointType;

public:
    AsioDatagramChannel(ChannelFactory* factory,
                        ChannelPipeline* pipeline,
                        ChannelSink* sink,
                        AsioServicePool::IOService& ioService,
                        int ioThreadCount,
                        int ipProtocol);

    boost::asio::ip::udp::socket& getSocket() { return this->udpSocket; }
    AsioServicePool::IOService& getIOService() { return ioService; }
    ChannelBuffer& getReadBuffer() { return *readBuffer; }

    virtual const SocketAddress& getLocalAddress() const;
    virtual const SocketAddress& getRemoteAddress() const;

    virtual bool isBound() const {
        return isOpen() && udpSocket.is_open();
    }

    virtual bool isConnected() const {
        return true;
    }

    virtual ChannelConfig& getConfig();
    virtual const ChannelConfig& getConfig() const;

    virtual int getInterestOps() const;
    virtual int getRawInterestOps() const;

    void setRawInterestOpsNow(int interestOps);

    virtual ChannelFuturePtr write(const ChannelMessage& message,
                                   const SocketAddress& remoteAddress);

    void sendto(const MessageEvent& evt);
    void cleanUpWriteBuffer();

    void handleReceiveFrom(const boost::system::error_code& error, size_t bytes_transferred);
    void handleSendTo(const boost::system::error_code& error, size_t bytes_transferred);
    
    virtual void joinGroup(const IpAddress& multicastAddress);
    virtual void joinGroup(const IpAddress& multicastAddress, const NetworkInterface& networkInterface);
    virtual void leaveGroup(const IpAddress& multicastAddress);
    virtual void leaveGroup(const IpAddress& multicastAddress, const NetworkInterface& networkInterface);

    bool setClosed();

    boost::asio::ip::udp::endpoint& getEndpoint();

    void resetRemoteAddress() {
        remoteAddress = SocketAddress::NULL_ADDRESS;
    }
    
private:
    int ioThreadCount;

    AsioServicePool::IOService& ioService;
    boost::asio::ip::udp::socket udpSocket;

    DefaultAsioDatagramChannelConfig config;

    ChannelBufferPtr readBuffer;
    AsioWriteOperationQueue writeQueue;

    handler_allocator<int> readAllocator;
    handler_allocator<int> writeAllocator;

    mutable SocketAddress localAddress;
    mutable SocketAddress remoteAddress;

    SocketAddress::SmartPointer remoteAddressImplPr;
};

}}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIODATAGRAMCHANNEL_H)
