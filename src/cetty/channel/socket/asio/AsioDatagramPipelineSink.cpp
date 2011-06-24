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
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "cetty/buffer/ChannelBuffer.h"

#include "cetty/channel/ChannelEvent.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ChannelState.h"
#include "cetty/channel/ChannelStateEvent.h"
#include "cetty/channel/socket/asio/AsioDatagramChannel.h"
#include "cetty/channel/socket/asio/AsioDatagramPipelineSink.h"
#include "cetty/channel/socket/asio/AsioDatagramChannelFactory.h"


namespace cetty { namespace channel { namespace socket { namespace asio { 

using namespace cetty::channel;
using namespace cetty::buffer;

void AsioDatagramPipelineSink::writeRequested(const cetty::channel::ChannelPipeline& pipeline,
                                              const cetty::channel::MessageEvent& e) {
    static_cast<AsioDatagramChannel*>(&(e.getChannel()))->sendto(e);
}

void AsioDatagramPipelineSink::stateChangeRequested(const cetty::channel::ChannelPipeline& pipeline,
                                                    const cetty::channel::ChannelStateEvent& e) {
    handleStateChange(*static_cast<AsioDatagramChannel*>(&(e.getChannel())), e);
}

void AsioDatagramPipelineSink::handleStateChange(AsioDatagramChannel& channel, const ChannelStateEvent& evt) {
    const ChannelState& state = evt.getState();
    const boost::any& value = evt.getValue();
    const ChannelFuturePtr& future = evt.getFuture();

    if (state == ChannelState::OPEN) {
        if (value.empty()) {
            close(channel, future);
        }
    }
    else if (state == ChannelState::BOUND) {
        if (value.empty()) {
            close(channel, future);
        }
        else {
            const SocketAddress* address = boost::any_cast<SocketAddress>(&value);
            if (address) {
                bind(channel, future, *address);
            }
            else {
                close(channel, future);
            }
        }
    }
    else if (state == ChannelState::CONNECTED) {
        if (value.empty()) {
            //disconnect(channel, future);
        }
        else {
            const SocketAddress* address = boost::any_cast<SocketAddress>(&value);
            if (address) {
                connect(channel, future, *address);
            }
            else {
                close(channel, future);
            }
        }
    }
    else if (state == ChannelState::INTEREST_OPS) {
        //setInterestOps(channel, future, ((Integer) value).intValue());
    }
}

void AsioDatagramPipelineSink::close(AsioDatagramChannel& channel, const ChannelFuturePtr& future) {
    if (!channel.isOpen() || !channel.getSocket().is_open()) {
        return;
    }

    try {
        channel.getSocket().close();

        if (channel.setClosed()) {
            future->setSuccess();

            if (channel.isBound()) {
                Channels::fireChannelUnbound(channel);
            }

            channel.cleanUpWriteBuffer();
            Channels::fireChannelClosed(channel);
        }
        else {
            future->setSuccess();
        }
    }
    catch (const boost::system::system_error& error) {
        IOException e("closing udp socket has thrown an exception", error.code().value());
        channel.setClosed();
        future->setFailure(e);
        Channels::fireExceptionCaught(channel, e);
    }
}


void AsioDatagramPipelineSink::bind(AsioDatagramChannel& channel, const ChannelFuturePtr& future, const SocketAddress& address) {
    bool bound = false;
    try {
        // First bind the DatagramSocket the specified port.
        boost::asio::ip::udp::endpoint endpoint(
            boost::asio::ip::address::from_string(address.address()), address.port());

        channel.getSocket().bind(endpoint);
        bound = true;

        future->setSuccess();
        Channels::fireChannelBound(channel, address);

        startChannel(channel);
    }
    catch (const Exception& e) {
        future->setFailure(e);
        Channels::fireExceptionCaught(channel, e);

        if (bound) {
            close(channel, future);
        }
    }
}

void AsioDatagramPipelineSink::connect(AsioDatagramChannel& channel, const ChannelFuturePtr& future, const SocketAddress& remoteAddress) {
    bool bound = channel.isBound();
    bool connected = false;

    future->addListener(ChannelFutureListener::CLOSE_ON_FAILURE);

    // Clear the cached address so that the next getRemoteAddress() call
    // updates the cache.
    channel.resetRemoteAddress();

    try {
        boost::asio::ip::udp::endpoint endpoint(
            boost::asio::ip::address::from_string(remoteAddress.address()), remoteAddress.port());

        channel.getSocket().connect(endpoint);
        connected = true;

        // Fire events.
        future->setSuccess();
        if (!bound) {
            Channels::fireChannelBound(channel, channel.getLocalAddress());
        }
        Channels::fireChannelConnected(channel, channel.getRemoteAddress());

        if (!bound) {
            startChannel(channel);
        }
    }
    catch (const Exception& e) {
        future->setFailure(e);
        Channels::fireExceptionCaught(channel, e);

        if (connected) {
            close(channel, future);
        }
    }
}

void AsioDatagramPipelineSink::startChannel(AsioDatagramChannel &channel) {
    Array arry;
    channel.getReadBuffer().writableBytes(arry);

    channel.getSocket().async_receive_from(
        boost::asio::buffer(arry.data(), arry.length()),
        channel.getEndpoint(),
        boost::bind(&AsioDatagramChannel::handleReceiveFrom,
        &channel,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));

    if (ioServicePool.isSingleThread()) {
        AsioDatagramChannelFactory* factory
            = dynamic_cast<AsioDatagramChannelFactory*>(&channel.getFactory());

        factory->start();
    }
}

}}}}