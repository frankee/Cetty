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

#include "cetty/bootstrap/ServerBootstrap.h"

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelConfig.h"
#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/ServerChannelFactory.h"
#include "cetty/channel/Channels.h"
#include "cetty/channel/ChannelException.h"
#include "cetty/channel/ExceptionEvent.h"
#include "cetty/channel/ChannelStateEvent.h"
#include "cetty/channel/ChildChannelStateEvent.h"
#include "cetty/channel/DefaultChannelPipeline.h"
#include "cetty/channel/socket/ServerSocketChannelFactory.h"

#include "cetty/util/Exception.h"

namespace cetty { namespace bootstrap {

using namespace cetty::channel;
using namespace cetty::channel::socket;
using namespace cetty::util;

void ServerBootstrap::setFactory(const ChannelFactoryPtr& factory) {
    if (boost::dynamic_pointer_cast<ServerChannelFactory>(factory)) {
        Bootstrap::setFactory(factory);
    }
    else {
        throw InvalidArgumentException(
            "factory must be a ServerChannelFactory");
    }
}

Channel* ServerBootstrap::bind() {
    const SocketAddress* localAddress = getTypedOption<SocketAddress>("localAddress");
    if (NULL == localAddress) return NULL;

    return bind(*localAddress);
}

Channel* ServerBootstrap::bind(const SocketAddress& localAddress) {
    // bossPipeline's life cycle will be managed by the server channel.
    ChannelPipeline* bossPipeline = Channels::pipeline();

    BinderPtr binder = BinderPtr(new Binder(localAddress, *this));
    ChannelHandlerPtr binderHandler = boost::dynamic_pointer_cast<ChannelHandler>(binder);
    ChannelHandlerPtr parentHandler = getParentHandler();

    bossPipeline->addLast("binder", binderHandler);
    if (parentHandler) {
        bossPipeline->addLast("userHandler", parentHandler->clone());
    }

    const ChannelFactoryPtr& factory = getFactory();
    ServerSocketChannelFactoryPtr serverFactory =
        boost::dynamic_pointer_cast<ServerSocketChannelFactory>(factory);
    if (serverFactory) {
        serverFactory->setIpProtocolVersion(localAddress.family());
    }

    Channel* channel = getFactory()->newChannel(bossPipeline);

    // Wait until the future is available.
    Binder::FutureQueue& futureQueue = binder->getFutureQueue();
    ChannelFuturePtr future;
    
    do {
        if (!futureQueue.empty()) {
            future = futureQueue.front();
        }

        //boost::this_thread::sleep(boost::posix_time::millisec(10));
        boost::this_thread::yield();
        
        if (boost::this_thread::interruption_requested()) {
            return NULL; 
        }
    }
    while (!future);

    // Wait for the future.
    future->awaitUninterruptibly();
    if (!future->isSuccess()) {
        future->getChannel().close()->awaitUninterruptibly();
        throw ChannelException(
            std::string("Failed to bind to: ") + localAddress.toString());
    }

    return channel;
}

void ServerBootstrap::Binder::channelOpen(ChannelHandlerContext& ctx,
                                          const ChannelStateEvent& evt) {
    if (bootstrap.getPipelineFactory()) {
        evt.getChannel().getConfig().setPipelineFactory(
                                        bootstrap.getPipelineFactory());
    }

    // Split options into two categories: parent and child.
    OptionsMap& allOptions = bootstrap.getOptions();
    OptionsMap parentOptions;
    OptionsMap::iterator itr;
    for (itr = allOptions.begin(); itr != allOptions.end(); ++itr) {
        if (itr->first.find("child.") == 0) {
            childOptions.insert(std::make_pair(itr->first.substr(6), itr->second));
        }
        else if (itr->first.compare("pipelineFactory") != 0) {
            parentOptions.insert(std::make_pair(itr->first, itr->second));
        }
    }

    // Apply parent options.
    evt.getChannel().getConfig().setOptions(parentOptions);
    ctx.sendUpstream(evt);

    futureQueue.push_back(evt.getChannel().bind(localAddress));
}

void ServerBootstrap::Binder::childChannelOpen(ChannelHandlerContext& ctx,
                                               const ChildChannelStateEvent& e) {
    // Apply child options.
    e.getChildChannel().getConfig().setOptions(childOptions);
    ctx.sendUpstream(e);
}

void ServerBootstrap::Binder::exceptionCaught(ChannelHandlerContext& ctx,
                                              const ExceptionEvent& e) {
    futureQueue.push_back(Channels::failedFuture(e.getChannel(), e.getCause()));
    ctx.sendUpstream(e);
}

}}