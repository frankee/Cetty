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

#include <stdarg.h>
#include <boost/assert.hpp>

#include "cetty/util/Exception.h"
#include "cetty/util/internal/ConversionUtil.h"

#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelPipelineFactory.h"
#include "cetty/channel/ChannelState.h"
#include "cetty/channel/UpstreamChannelStateEvent.h"
#include "cetty/channel/DownstreamChannelStateEvent.h"
#include "cetty/channel/UpstreamMessageEvent.h"
#include "cetty/channel/DownstreamMessageEvent.h"
#include "cetty/channel/DefaultChannelFuture.h"
#include "cetty/channel/DefaultChannelPipeline.h"
#include "cetty/channel/DefaultChildChannelStateEvent.h"
#include "cetty/channel/DefaultWriteCompletionEvent.h"
#include "cetty/channel/DefaultExceptionEvent.h"
#include "cetty/channel/FailedChannelFuture.h"
#include "cetty/channel/AbstractChannel.h"
#include "cetty/channel/SucceededChannelFuture.h"
#include "cetty/channel/Channels.h"

namespace cetty { namespace channel {

using namespace cetty::util;
using namespace cetty::util::internal;

class CloneChannelPipelineFactory : public ChannelPipelineFactory {
public:
    CloneChannelPipelineFactory(ChannelPipeline* pipeline)
        : cloneFromPipeline(pipeline) {
    }
    virtual ~CloneChannelPipelineFactory() {}

    ChannelPipeline* getPipeline() {
        return Channels::pipeline(cloneFromPipeline);
    }

private:
    ChannelPipeline* cloneFromPipeline;
};

ChannelPipeline* Channels::pipeline() {
    return new DefaultChannelPipeline();
}

ChannelPipeline* Channels::pipeline(ChannelPipeline* pipeline) {
    ChannelPipeline* newPipeline = Channels::pipeline();
    BOOST_ASSERT(newPipeline);

    ChannelPipeline::ChannelHandlers handlers = pipeline->toMap();
    ChannelPipeline::ChannelHandlers::iterator itr = handlers.begin();
    for (; itr != handlers.end(); ++itr) {
        newPipeline->addLast(itr->first, itr->second->clone());
    }
    return newPipeline;
}

ChannelPipeline* Channels::pipeline(const ChannelHandlerPtr& handler) {
    ChannelPipeline* newPipeline = pipeline();
    if (handler) { newPipeline->addLast("0", handler); }
    return newPipeline;
}

ChannelPipeline* Channels::pipeline(const ChannelHandlerPtr& handler0,
                                    const ChannelHandlerPtr& handler1) {
    ChannelPipeline* newPipeline = pipeline();
    if (handler0) { newPipeline->addLast("0", handler0); }
    if (handler1) { newPipeline->addLast("1", handler1); }
    return newPipeline;
}

ChannelPipeline* Channels::pipeline(const ChannelHandlerPtr& handler0,
                                    const ChannelHandlerPtr& handler1,
                                    const ChannelHandlerPtr& handler2) {
    ChannelPipeline* newPipeline = pipeline();
    if (handler0) { newPipeline->addLast("0", handler0); }
    if (handler1) { newPipeline->addLast("1", handler1); }
    if (handler1) { newPipeline->addLast("2", handler2); }
    return newPipeline;
}

ChannelPipeline* Channels::pipeline(const ChannelHandlerPtr& handler0,
                                    const ChannelHandlerPtr& handler1,
                                    const ChannelHandlerPtr& handler2,
                                    const ChannelHandlerPtr& handler3) {
    ChannelPipeline* newPipeline = pipeline();
    if (handler0) { newPipeline->addLast("0", handler0); }
    if (handler1) { newPipeline->addLast("1", handler1); }
    if (handler1) { newPipeline->addLast("2", handler2); }
    if (handler1) { newPipeline->addLast("3", handler3); }
    return newPipeline;
}

ChannelPipeline* Channels::pipeline(const std::vector<ChannelHandlerPtr>& handlers) {
    ChannelPipeline* newPipeline = Channels::pipeline();
    BOOST_ASSERT(newPipeline);

    for (size_t i = 0; i < handlers.size(); ++i) {
        const ChannelHandlerPtr& h = handlers[i];
        if (!h) {
            continue;
        }
        newPipeline->addLast(ConversionUtil::toString((int)i), handlers[i]);
    }
    
    return newPipeline;
}

ChannelPipelineFactoryPtr Channels::pipelineFactory(ChannelPipeline* pipeline ) {
    return ChannelPipelineFactoryPtr(new CloneChannelPipelineFactory(pipeline));
}

ChannelFuturePtr Channels::future(Channel& channel, bool cancellable) {
    return ChannelFuturePtr(new DefaultChannelFuture(channel, cancellable));
}

ChannelFuturePtr Channels::failedFuture(Channel& channel, const Exception& cause) {
    return ChannelFuturePtr(new FailedChannelFuture(channel, cause));
}

void Channels::fireChannelOpen(Channel& channel) {
    // Notify the parent handler.
    Channel* parent = channel.getParent();
    if (parent != NULL) {
        fireChildChannelStateChanged(*parent, channel);
    }

    channel.getPipeline().sendUpstream(UpstreamChannelStateEvent(
        channel, ChannelState::OPEN, boost::any(true)));
}

void Channels::fireChannelOpen(ChannelHandlerContext& ctx) {
    ctx.sendUpstream(UpstreamChannelStateEvent(
        ctx.getChannel(), ChannelState::OPEN, boost::any(true)));
}

void Channels::fireChannelBound(Channel& channel, const SocketAddress& localAddress) {
    channel.getPipeline().sendUpstream(UpstreamChannelStateEvent(
        channel, ChannelState::BOUND, boost::any(&localAddress)));
}

void Channels::fireChannelBound(ChannelHandlerContext& ctx, const SocketAddress& localAddress) {
    ctx.sendUpstream(UpstreamChannelStateEvent(
        ctx.getChannel(), ChannelState::BOUND, boost::any(&localAddress)));
}

void Channels::fireChannelConnected(Channel& channel, const SocketAddress& remoteAddress) {
    channel.getPipeline().sendUpstream(UpstreamChannelStateEvent(
        channel, ChannelState::CONNECTED, boost::any(&remoteAddress)));
}

void Channels::fireChannelConnected(ChannelHandlerContext& ctx, const SocketAddress& remoteAddress) {
    ctx.sendUpstream(UpstreamChannelStateEvent(
        ctx.getChannel(), ChannelState::CONNECTED, boost::any(&remoteAddress)));
}

void Channels::fireMessageReceived(Channel& channel, const ChannelMessage& message) {
    channel.getPipeline().sendUpstream(
        UpstreamMessageEvent(channel, message, channel.getRemoteAddress()));
}

void Channels::fireMessageReceived(Channel& channel, const ChannelMessage& message, const SocketAddress& remoteAddress) {
    channel.getPipeline().sendUpstream(
        UpstreamMessageEvent(channel, message, remoteAddress));
}

void Channels::fireMessageReceived(ChannelHandlerContext& ctx, const ChannelMessage& message) {
    Channel& channel = ctx.getChannel();
    ctx.sendUpstream(
        UpstreamMessageEvent(channel, message, channel.getRemoteAddress()));
}

void Channels::fireMessageReceived(ChannelHandlerContext& ctx, const ChannelMessage& message, const SocketAddress& remoteAddress) {
    ctx.sendUpstream(
        UpstreamMessageEvent(ctx.getChannel(), message, remoteAddress));
}

void Channels::fireWriteCompleted(Channel& channel, long amount) {
    if (amount == 0) return;
    
    channel.getPipeline().sendUpstream(DefaultWriteCompletionEvent(channel, amount));
}

void Channels::fireWriteCompleted(ChannelHandlerContext& ctx, long amount) {
    ctx.sendUpstream(DefaultWriteCompletionEvent(ctx.getChannel(), amount));
}

void Channels::fireChannelInterestChanged(Channel& channel, int interestOps) {
    channel.getPipeline().sendUpstream(UpstreamChannelStateEvent(
        channel, ChannelState::INTEREST_OPS, boost::any(interestOps)));
}

void Channels::fireChannelInterestChanged(ChannelHandlerContext& ctx, int interestOps) {
    ctx.sendUpstream(UpstreamChannelStateEvent(
        ctx.getChannel(), ChannelState::INTEREST_OPS, boost::any(interestOps)));
}

void Channels::fireChannelDisconnected(Channel& channel) {
    channel.getPipeline().sendUpstream(UpstreamChannelStateEvent(
        channel, ChannelState::CONNECTED, boost::any()));
}

void Channels::fireChannelDisconnected(ChannelHandlerContext& ctx) {
    ctx.sendUpstream(UpstreamChannelStateEvent(
        ctx.getChannel(), ChannelState::CONNECTED, boost::any()));
}

void Channels::fireChannelUnbound(Channel& channel) {
    channel.getPipeline().sendUpstream(UpstreamChannelStateEvent(
        channel, ChannelState::BOUND, boost::any()));
}

void Channels::fireChannelUnbound(ChannelHandlerContext& ctx) {
    ctx.sendUpstream(UpstreamChannelStateEvent(
        ctx.getChannel(), ChannelState::BOUND, boost::any()));
}

void Channels::fireChannelClosed(Channel& channel) {
    channel.getPipeline().sendUpstream(UpstreamChannelStateEvent(
        channel, ChannelState::OPEN, boost::any()));

    // Notify the parent handler.
    Channel* parent = channel.getParent();
    if (parent != NULL) {
        fireChildChannelStateChanged(*parent, channel);
    }
}

void Channels::fireChannelClosed(ChannelHandlerContext& ctx) {
    ctx.sendUpstream(UpstreamChannelStateEvent(
        ctx.getChannel(), ChannelState::OPEN, boost::any()));
}

void Channels::fireExceptionCaught(Channel& channel, const Exception& cause) {
    channel.getPipeline().sendUpstream(
        DefaultExceptionEvent(channel, cause));
}

void Channels::fireExceptionCaught(ChannelHandlerContext& ctx, const Exception& cause) {
    ctx.sendUpstream(DefaultExceptionEvent(ctx.getChannel(), cause));
}

ChannelFuturePtr Channels::bind(Channel& channel, const SocketAddress& localAddress) {
    ChannelFuturePtr future = Channels::future(channel);
    channel.getPipeline().sendDownstream(DownstreamChannelStateEvent(
        channel, future, ChannelState::BOUND, boost::any(localAddress)));
    return future;
}

void Channels::bind(ChannelHandlerContext& ctx, const ChannelFuturePtr& future, const SocketAddress& localAddress) {
    ctx.sendDownstream(DownstreamChannelStateEvent(
        ctx.getChannel(), future, ChannelState::BOUND, boost::any(localAddress)));
}

ChannelFuturePtr Channels::unbind(Channel& channel) {
    return channel.unbind();
}

void Channels::unbind(ChannelHandlerContext& ctx, const ChannelFuturePtr& future) {
    ctx.sendDownstream(DownstreamChannelStateEvent(
        ctx.getChannel(), future, ChannelState::BOUND));
}

ChannelFuturePtr Channels::connect(Channel& channel, const SocketAddress& remoteAddress) {
    ChannelFuturePtr future = Channels::future(channel, true);

    channel.getPipeline().sendDownstream(DownstreamChannelStateEvent(
        channel, future, ChannelState::CONNECTED, boost::any(remoteAddress)));

    return future;
}

void Channels::connect(ChannelHandlerContext& ctx, const ChannelFuturePtr& future, const SocketAddress& remoteAddress) {
    ctx.sendDownstream(DownstreamChannelStateEvent(
        ctx.getChannel(), future, ChannelState::CONNECTED, boost::any(remoteAddress)));
}

ChannelFuturePtr Channels::write(Channel& channel,
                                 const ChannelMessage& message,
                                 bool withFuture) {
    return channel.write(message, withFuture);
}

void Channels::write(ChannelHandlerContext& ctx,
                     const ChannelFuturePtr& future,
                     const ChannelMessage& message) {
    write(ctx, future, message, ctx.getChannel().getRemoteAddress());
}

ChannelFuturePtr Channels::write(Channel& channel,
                                 const ChannelMessage& message,
                                 const SocketAddress& remoteAddress,
                                 bool  withFuture) {
    return channel.write(message, remoteAddress, withFuture);
}

void Channels::write(ChannelHandlerContext& ctx,
                     const ChannelFuturePtr& future,
                     const ChannelMessage& message,
                     const SocketAddress& remoteAddress) {
    ctx.sendDownstream(
        DownstreamMessageEvent(ctx.getChannel(), future, message, remoteAddress));
}

ChannelFuturePtr Channels::setInterestOps(Channel& channel, int interestOps) {
    return channel.setInterestOps(interestOps);
}

void Channels::setInterestOps(ChannelHandlerContext& ctx,
                              const ChannelFuturePtr& future,
                              int interestOps) {
    validateInterestOps(interestOps);
    interestOps = filterDownstreamInterestOps(interestOps);

    ctx.sendDownstream(DownstreamChannelStateEvent(
        ctx.getChannel(), future, ChannelState::INTEREST_OPS, boost::any(interestOps)));
}

ChannelFuturePtr Channels::disconnect(Channel& channel) {
    return channel.disconnect();
}

void Channels::disconnect(ChannelHandlerContext& ctx, const ChannelFuturePtr& future) {
    ctx.sendDownstream(DownstreamChannelStateEvent(
        ctx.getChannel(), future, ChannelState::CONNECTED));
}

ChannelFuturePtr Channels::close(Channel& channel) {
    return channel.close();
}

void Channels::close(ChannelHandlerContext& ctx, const ChannelFuturePtr& future) {
    ctx.sendDownstream(DownstreamChannelStateEvent(
        ctx.getChannel(), future, ChannelState::OPEN));
}

void Channels::fireChildChannelStateChanged(Channel& channel, Channel& childChannel) {
    channel.getPipeline().sendUpstream(
        DefaultChildChannelStateEvent(channel, childChannel));
}

void Channels::validateInterestOps(int interestOps) {
    switch (interestOps) {
        case Channel::OP_NONE:
        case Channel::OP_READ:
        case Channel::OP_WRITE:
        case Channel::OP_READ_WRITE:
            break;
        default:
            throw InvalidArgumentException(
                std::string("Invalid interestOps: ") + Integer::toString(interestOps));
    }
}

int Channels::filterDownstreamInterestOps(int interestOps) {
    return interestOps & (~Channel::OP_WRITE);
}

int Channels::validateAndFilterDownstreamInteresOps(int interestOps) {
    validateInterestOps(interestOps);
    return filterDownstreamInterestOps(interestOps);
}


}}