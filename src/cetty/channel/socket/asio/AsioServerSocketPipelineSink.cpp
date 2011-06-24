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
#include "cetty/channel/socket/asio/AsioServerSocketPipelineSink.h"

#include <boost/bind.hpp>

#include "cetty/channel/IpAddress.h"
#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ChannelState.h"
#include "cetty/channel/ChannelStateEvent.h"
#include "cetty/channel/ChannelPipelineFactory.h"
#include "cetty/channel/socket/asio/AsioServicePool.h"
#include "cetty/channel/socket/asio/AsioAcceptedSocketChannel.h"
#include "cetty/channel/socket/asio/AsioServerSocketChannel.h"

#include "cetty/logging/InternalLogger.h"
#include "cetty/logging/InternalLoggerFactory.h"

#include "cetty/util/internal/ConversionUtil.h"

namespace cetty { namespace channel { namespace socket { namespace asio {

using namespace cetty::channel;
using namespace cetty::logging;
using namespace cetty::util::internal;

InternalLogger* AsioServerSocketPipelineSink::logger =
                    InternalLoggerFactory::getInstance("AsioServerSocketPipelineSink");

AsioServerSocketPipelineSink::Boss::Boss(AsioServerSocketPipelineSink& sink,
                                         AsioServerSocketChannel& channel,
                                         AsioServicePool& ioServicePool,
                                         boost::asio::ip::tcp::acceptor& acceptor)
    : ioServicePool(ioServicePool),
      acceptor(acceptor),
      serverChannel(channel),
      sink(sink) {
}

void AsioServerSocketPipelineSink::Boss::run() {
    ChannelPipeline* pipeline =
        serverChannel.getConfig().getPipelineFactory()->getPipeline();

    AsioServicePool::IOService& ioService = ioServicePool.getIOService();
    AsioAcceptedSocketChannel* acceptedChannel
        = new AsioAcceptedSocketChannel(&serverChannel,
                                        &(serverChannel.getFactory()),
                                        pipeline,
                                        &sink,
                                        ioService,
                                        ioServicePool.getThreadId(ioService.index()));

    acceptor.async_accept(acceptedChannel->getSocket(),
        make_custom_alloc_handler(acceptAllocator,
            boost::bind(&AsioServerSocketPipelineSink::Boss::handleAccept,
                        this,
                        boost::asio::placeholders::error,
                        acceptedChannel)));
}

void AsioServerSocketPipelineSink::Boss::handleAccept(
                                            const boost::system::error_code& error,
                                            AsioAcceptedSocketChannel* channel) {
    BOOST_ASSERT(channel);
    if (!error) {
        if (!channel->start()) {
            // has no local address or remote address
            // may never happened.
            delete channel;
        }

		sink.serverChildrenChannels().insert(
						std::make_pair<Integer, AsioSocketChannel*>(channel->getId(), channel));

        ChannelPipeline* pipeline =
            serverChannel.getConfig().getPipelineFactory()->getPipeline();

        AsioServicePool::IOService& ioService = ioServicePool.getIOService();
        channel = new AsioAcceptedSocketChannel(
                                    &serverChannel,
                                    &(serverChannel.getFactory()),
                                    pipeline,
                                    &sink,
                                    ioService,
                                    ioServicePool.getThreadId(ioService.index()));

        acceptor.async_accept(channel->getSocket(),
            make_custom_alloc_handler(acceptAllocator,
                boost::bind(&AsioServerSocketPipelineSink::Boss::handleAccept,
                            this,
                            boost::asio::placeholders::error,
                            channel)));
    }
    else {
        delete channel;
        logger->warn(
            std::string("Failed to accept a connection any more. ErrorCode:") +
            Integer::toString(error.value()));
    }
}

void AsioServerSocketPipelineSink::writeRequested(const cetty::channel::ChannelPipeline& pipeline,
                                                  const cetty::channel::MessageEvent& e) {
    Channel& channel = e.getChannel();
    (static_cast<AsioSocketChannel*>(&channel))->write(e);
}

void AsioServerSocketPipelineSink::stateChangeRequested(const cetty::channel::ChannelPipeline& pipeline,
                                                        const cetty::channel::ChannelStateEvent& e) {
    Channel& channel = e.getChannel();
    if (channel.getParent()) {
        handleStateChange(*static_cast<AsioSocketChannel*>(&channel), e);
    }
    else {
        handleStateChange(*static_cast<AsioServerSocketChannel*>(&channel), e);
    }
}

void AsioServerSocketPipelineSink::handleServerSocket(
                                    AsioServerSocketChannel& channel,
                                    const ChannelEvent& e) {
    const ChannelStateEvent* evt = dynamic_cast<const ChannelStateEvent*>(&e);
    if (NULL == evt) return;

    handleStateChange(channel, *evt);
}

void AsioServerSocketPipelineSink::handleAcceptedSocket(AsioSocketChannel& channel,
                                                        const ChannelEvent& e) {
    const ChannelStateEvent* stateEvt = dynamic_cast<const ChannelStateEvent*>(&e);
    if (stateEvt) {
        handleStateChange(channel, *stateEvt);
        return;
    }

    const MessageEvent* msgEvt = dynamic_cast<const MessageEvent*>(&e);
    if (msgEvt) {
        channel.write(*msgEvt);
    }
}

void AsioServerSocketPipelineSink::handleStateChange(AsioServerSocketChannel& channel,
                                                     const ChannelStateEvent& evt) {
    const ChannelFuturePtr& future = evt.getFuture();
    const ChannelState& state = evt.getState();
    const boost::any& value = evt.getValue();

    if (state == ChannelState::OPEN) {
        if (value.empty()) {
            closeServerChannel(channel, future);
        }
    }
    else if (state == ChannelState::BOUND) {
        if (value.empty()) {
            closeServerChannel(channel, future);
        }
        else {
            const SocketAddress* address = boost::any_cast<SocketAddress>(&value);
            if (address) {
                bind(channel, future, *address);
            }
            else {
                closeServerChannel(channel, future);
            }
        }
    }
}

void AsioServerSocketPipelineSink::handleStateChange(AsioSocketChannel& channel,
                                                     const ChannelStateEvent& evt) {
    const ChannelFuturePtr& future = evt.getFuture();
    const ChannelState& state = evt.getState();
    const boost::any& value = evt.getValue();

    if (ChannelState::INTEREST_OPS == state) {
        channel.getIOService().service().post(
            boost::bind(AsioSocketChannel::setInterestOps,
                        &channel,
                        future,
                        ConversionUtil::toInt(value)));
        //channel.setInterestOps(future, ConversionUtil::toInt(value));
    }
    else {
        // when AsioAcceptedSocketChannel started, it has connected. So it will has
        // no more OPEN, BOUND, CONNECTED event, but only CLOSE, UNBOUND, DISCONNECTED event.
        if (value.empty()) {
            closeAcceptChannel(channel, future);
        }
    }
}

void AsioServerSocketPipelineSink::bind(AsioServerSocketChannel& channel,
                                        const ChannelFuturePtr& future,
                                        const SocketAddress& localAddress) {
    bool bound = false;
    bool bossStarted = false;

    try {
        boost::asio::ip::tcp::endpoint ep(
            boost::asio::ip::address::from_string(localAddress.address()),
            localAddress.port());
        DefaultAsioServerSocketChannelConfig* config =
            dynamic_cast<DefaultAsioServerSocketChannelConfig*>(&channel.getConfig());

        acceptor.bind(ep);
        acceptor.listen(config->getBacklog());

        bound = true;
        Channels::fireChannelBound(channel, channel.getLocalAddress());

        boss = new Boss(*this, channel, ioServicePool, acceptor);
        boss->run();
        bossStarted = true;

        future->setSuccess();
    }
    catch (const std::exception& e) {
        Exception exception(e.what());
        future->setFailure(exception);
        Channels::fireExceptionCaught(channel, exception);
    }

    if (!bossStarted && bound) {
        closeServerChannel(channel, future);
    }
}

void AsioServerSocketPipelineSink::closeServerChannel(
                                        AsioServerSocketChannel& channel,
                                        const ChannelFuturePtr& future) {
    bool bound = channel.isBound();
    try {
        if (channel.getAcceptor().is_open()) {
            channel.getAcceptor().close();
        }

        // Make sure the boss thread is not running so that that the future
        // is notified after a new connection cannot be accepted anymore.
        // See NETTY-256 for more information.
        if (channel.setClosed()) {
            future->setSuccess();
            if (bound) {
                Channels::fireChannelUnbound(channel);
            }
            Channels::fireChannelClosed(channel);
        }
        else {
            future->setSuccess();
        }
    }
    catch (const std::exception& t) {
        Exception e(t.what());
        future->setFailure(e);
        Channels::fireExceptionCaught(channel, e);
    }

	//close all children Channels
	while (!clildrenChannels.empty()) {
		AsioSocketChannel* child = clildrenChannels.begin()->second;
		if (child) {
			closeAcceptChannel(*child, child->getCloseFuture());
		}
		else {
			clildrenChannels.erase(clildrenChannels.begin());
		}
	}
}

void AsioServerSocketPipelineSink::closeAcceptChannel(
                                        AsioSocketChannel& channel,
                                        const ChannelFuturePtr& future) {
    channel.close(future);

	//delete the channel.
	ChildrenChannels::iterator itr = clildrenChannels.find(channel.getId());
	if (itr != clildrenChannels.end()) {
		delete itr->second;
		clildrenChannels.erase(itr);
	}
}

}}}}
