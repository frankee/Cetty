#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVERSOCKETPIPELINESINK_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVERSOCKETPIPELINESINK_H

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

#include "cetty/channel/AbstractChannelSink.h"
#include "cetty/channel/ChannelFuture.h"

#include "cetty/buffer/ChannelBuffer.h"
#include "cetty/util/Integer.h"

#include "cetty/channel/socket/asio/handler_allocator.hpp"

namespace cetty { namespace channel {
class MessageEvent;
class SocketAddress;
}}

namespace cetty { namespace logging {
class InternalLogger;
}}

namespace cetty { namespace channel { namespace socket { namespace asio {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::logging;

class AsioServicePool;
class AsioSocketChannel;
class AsioAcceptedSocketChannel;
class AsioServerSocketChannel;

class AsioServerSocketPipelineSink : public cetty::channel::AbstractChannelSink {
private:
	typedef std::map<Integer, AsioSocketChannel*> ChildrenChannels;

    class Boss {
    public:
        Boss(AsioServerSocketPipelineSink& sink,
             AsioServerSocketChannel& channel,
             AsioServicePool& ioServicePool,
             boost::asio::ip::tcp::acceptor& acceptor);

        // exceptions will be handled by the caller.
        void run();
        void handleAccept(const boost::system::error_code& error,
                          AsioAcceptedSocketChannel* channel);

    private:
        AsioServicePool& ioServicePool;
        boost::asio::ip::tcp::acceptor& acceptor;
        AsioServerSocketChannel& serverChannel;
        AsioServerSocketPipelineSink& sink;
        handler_allocator<int> acceptAllocator;
    };

public:
    AsioServerSocketPipelineSink(AsioServicePool& ioServicePool,
        boost::asio::ip::tcp::acceptor& acceptor)
        : boss(NULL), ioServicePool(ioServicePool), acceptor(acceptor) {
    }

    virtual ~AsioServerSocketPipelineSink() {
        if (boss) {
            delete boss;
        }
    }

    virtual void writeRequested(const cetty::channel::ChannelPipeline& pipeline,
                                const cetty::channel::MessageEvent& e);

    virtual void stateChangeRequested(const cetty::channel::ChannelPipeline& pipeline,
                                      const cetty::channel::ChannelStateEvent& e);
	
	ChildrenChannels serverChildrenChannels() { return clildrenChannels; }

private:
    void handleServerSocket(AsioServerSocketChannel& channel, const ChannelEvent& e);
    void handleAcceptedSocket(AsioSocketChannel& channel, const ChannelEvent& e);

    void handleStateChange(AsioServerSocketChannel& channel, const ChannelStateEvent& evt);
    void handleStateChange(AsioSocketChannel& channel, const ChannelStateEvent& evt);

    void bind(
        AsioServerSocketChannel& channel,
        const ChannelFuturePtr& future,
        const SocketAddress& localAddress);

    void closeServerChannel(AsioServerSocketChannel& channel,
                            const ChannelFuturePtr& future);

    void closeAcceptChannel(AsioSocketChannel& channel,
                            const ChannelFuturePtr& future);

private:
    static InternalLogger* logger;
    
private:
    Boss* boss;

    AsioServicePool& ioServicePool;
    boost::asio::ip::tcp::acceptor& acceptor;

	ChildrenChannels clildrenChannels;
};

}}}}


#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVERSOCKETPIPELINESINK_H)
