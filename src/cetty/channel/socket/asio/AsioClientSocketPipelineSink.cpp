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

#include "cetty/channel/socket/asio/AsioClientSocketPipelineSink.h"

#include <boost/bind.hpp>

#include "cetty/channel/ChannelState.h"
#include "cetty/channel/ChannelStateEvent.h"
#include "cetty/channel/AbstractChannelSink.h"
#include "cetty/channel/ChannelException.h"
#include "cetty/channel/IpAddress.h"
#include "cetty/channel/SocketAddress.h"

#include "cetty/channel/socket/asio/AsioClientSocketChannel.h"
#include "cetty/channel/socket/asio/AsioClientSocketChannelFactory.h"
#include "cetty/channel/socket/asio/AsioServicePool.h"
#include "cetty/channel/socket/asio/AsioWriteRequestQueue.h"

#include "cetty/logging/InternalLoggerFactory.h"
#include "cetty/util/internal/ConversionUtil.h"

namespace cetty { namespace channel { namespace socket { namespace asio { 

using namespace cetty::channel;
using namespace cetty::util;
using namespace cetty::util::internal;
using namespace cetty::logging;

InternalLogger* AsioClientSocketPipelineSink::logger
                    = InternalLoggerFactory::getInstance("AsioClientSocketPipelineSink");

void AsioClientSocketPipelineSink::writeRequested(const cetty::channel::ChannelPipeline& pipeline,
                                                  const cetty::channel::MessageEvent& e) {
    Channel& channel = e.getChannel();
    (static_cast<AsioSocketChannel*>(&channel))->write(e);
}

void AsioClientSocketPipelineSink::stateChangeRequested(const cetty::channel::ChannelPipeline& pipeline,
                                                        const cetty::channel::ChannelStateEvent& e) {
    Channel& channel = e.getChannel();
    handleStateChange(*static_cast<AsioSocketChannel*>(&channel), e);
}

void AsioClientSocketPipelineSink::handleStateChange(AsioSocketChannel& channel, const ChannelStateEvent& evt) {
    const ChannelFuturePtr& future = evt.getFuture();
    const ChannelState& state = evt.getState();
    const boost::any& value = evt.getValue();

    if (state == ChannelState::OPEN) {
        if (value.empty()) {
            channel.close(future);
        }
    }
    else if (state == ChannelState::BOUND) {
        if (value.empty()) {
            channel.close(future);
        }
    }
    else if (state == ChannelState::CONNECTED) {
        if (!value.empty()) {
            const SocketAddress* address = boost::any_cast<SocketAddress>(&value);
            if (address) {
                connect(channel, future, *address);
            }
        }
        else {
            channel.close(future);
        }
    }
    else if (state == ChannelState::INTEREST_OPS) {
        //TODO should be async operation
//        channel.getIOService().service().post(
//             boost::bind<void, const ChannelFuturePtr& , int>(
//                 &AsioSocketChannel::setInterestOps,
//                 &channel,
//                 future,
//                 ConversionUtil::toInt(value)));
        channel.setInterestOps(future, ConversionUtil::toInt(value));
    }
}

void AsioClientSocketPipelineSink::connect(AsioSocketChannel& channel,
                                           const ChannelFuturePtr& cf,
                                           const SocketAddress& remoteAddress) {
    try {
       boost::asio::ip::tcp::resolver resolver(ioServicePool.getIOService());
       boost::asio::ip::tcp::resolver::query query(
           remoteAddress.hostName(),
           Integer::toString(remoteAddress.port()));

       boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
       boost::asio::ip::tcp::endpoint endpoint = *iterator;
       channel.getSocket().async_connect(
           endpoint,
           boost::bind(&AsioSocketChannel::handleConnect,
                       &channel,
                       boost::asio::placeholders::error,
                       ++iterator,
                       cf));

        if (ioServicePool.isSingleThread()) {
            AsioClientSocketChannelFactory* factory
               = dynamic_cast<AsioClientSocketChannelFactory*>(&channel.getFactory());

            factory->start();
        }
    }
    catch (const Exception& e) {
       cf->setFailure(e);
       Channels::fireExceptionCaught(channel, e);
       channel.close(channel.getSucceededFuture());
    }
}

}}}}