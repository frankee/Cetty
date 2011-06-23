#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNEL_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNEL_H

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

#include <boost/any.hpp>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/detail/atomic_count.hpp>

#include "cetty/buffer/ChannelBuffer.h"
#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/DownstreamMessageEvent.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/channel/ChannelFuture.h"
#include "cetty/channel/ChannelPipeline.h"
#include "cetty/channel/socket/SocketChannel.h"
#include "cetty/channel/socket/asio/AsioServicePool.h"
#include "cetty/channel/socket/asio/DefaultAsioSocketChannelConfig.h"
#include "cetty/channel/socket/asio/AsioWriteRequestQueue.h"
#include "cetty/channel/socket/asio/handler_allocator.hpp"

namespace cetty { namespace channel  { namespace socket { namespace asio {

using namespace cetty::channel;
using namespace cetty::buffer;

class AsioSocketChannel : public cetty::channel::socket::SocketChannel {
public:
    AsioSocketChannel(Channel* parent,
                      ChannelFactory* factory,
                      ChannelPipeline* pipeline,
                      ChannelSink* sink,
                      AsioServicePool::IOService& ioService,
                      const boost::thread::id& id);

    virtual ~AsioSocketChannel();

    virtual ChannelConfig& getConfig() { return this->config; }
    virtual const ChannelConfig& getConfig() const { return this->config; }

    boost::asio::ip::tcp::socket& getSocket() {
        return this->tcpSocket;
    }

    AsioServicePool::IOService& getIOService() {
        return ioService;
    }

    virtual const SocketAddress& getLocalAddress() const;
    virtual const SocketAddress& getRemoteAddress() const;

    virtual bool isOpen() const {
        return state >= ST_CHANNEL_OPEN;
    }

    virtual bool isBound() const {
        return state >= ST_CHANNEL_BOUND;
    }

    virtual bool isConnected() const {
        return state == ST_CHANNEL_CONNECTED;
    }

	virtual int getInterestOps() const;

    virtual bool setClosed() {
        state = ST_CHANNEL_CLOSED;
        return AbstractChannel::setClosed();
    }

    void setBound() {
        BOOST_ASSERT(state == ST_CHANNEL_OPEN && "Invalid state.");
        state = ST_CHANNEL_BOUND;
    }

    void setConnected() {
        if (state != ST_CHANNEL_CLOSED) {
            state = ST_CHANNEL_CONNECTED;
        }
    }

    int getRawInterestOps() const {
        return AbstractChannel::getInterestOps();
    }

    void setRawInterestOpsNow(int interestOps) {
        AbstractChannel::setInterestOpsNow(interestOps);
    }

    virtual ChannelFuturePtr write(const ChannelMessage& message,
                                   bool  withFutrue = true);
    virtual ChannelFuturePtr write(const ChannelMessage& message,
                                   const SocketAddress& remoteAddress,
                                   bool  withFutrue = true);

    virtual ChannelFuturePtr unbind();
    virtual ChannelFuturePtr close();
    virtual ChannelFuturePtr disconnect();
    virtual ChannelFuturePtr setInterestOps(int interestOps);

    void write(const MessageEvent& evt);
    void close(const ChannelFuturePtr& future);
    void setInterestOps(const ChannelFuturePtr& future, int interestOps);
    void cleanUpWriteBuffer();

    void handleRead(const boost::system::error_code& error, size_t bytes_transferred);
    void handleWrite(const boost::system::error_code& error, size_t bytes_transferred);

    void handleConnect(const boost::system::error_code& error,
                       boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
                       const ChannelFuturePtr& cf);

private:
    void handleAtHighWaterMark();
    void handleAtLowWaterMark();

private:
    friend class AsioWriteOperationQueue;

protected:
    boost::thread::id threadId;

    AsioServicePool::IOService&  ioService;
    boost::asio::ip::tcp::socket tcpSocket;

    ChannelBufferPtr        readBuffer;
    AsioWriteOperationQueue writeQueue;
    bool isWriting;
    int  highWaterMarkCounter;

    DefaultAsioSocketChannelConfig config;

    handler_allocator<int> readAllocator;
    handler_allocator<int> writeAllocator;
    handler_allocator<boost::detail::atomic_count> ipcWriteAllocator;
    handler_allocator<boost::detail::atomic_count> ipcStateChangeAllocator;

    mutable SocketAddress localAddress;
    mutable SocketAddress remoteAddress;

private:
    static const int ST_CHANNEL_OPEN = 0;
    static const int ST_CHANNEL_BOUND = 1;
    static const int ST_CHANNEL_CONNECTED = 2;
    static const int ST_CHANNEL_CLOSED = -1;

    int state;
};

}}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNEL_H)
