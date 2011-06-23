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

#include "cetty/channel/SimpleChannelDownstreamHandler.h"
#include "cetty/channel/ChannelHandler.h"
#include "cetty/channel/ChannelHandlerContext.h"
#include "cetty/channel/ChannelState.h"
#include "cetty/channel/ChannelEvent.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ChannelStateEvent.h"

#include "cetty/logging/InternalLogger.h"
#include "cetty/logging/InternalLoggerFactory.h"

namespace cetty { namespace channel {

using namespace cetty::logging;

InternalLogger* SimpleChannelDownstreamHandler::logger =
    InternalLoggerFactory::getInstance("SimpleChannelDownstreamHandler");


void SimpleChannelDownstreamHandler::handleDownstream(ChannelHandlerContext& ctx,
                                                      const ChannelEvent& e) {
    ctx.sendDownstream(e);
}

void SimpleChannelDownstreamHandler::stateChangeRequested(ChannelHandlerContext& ctx,
                                                          const ChannelStateEvent& e) {
    const boost::any& value = e.getValue();
    const ChannelState& state = e.getState();

    if (state == ChannelState::OPEN) {
        if (value.empty()) {
            closeRequested(ctx, e);
        }
    }
    else if (state == ChannelState::BOUND) {
        if (value.empty()) {
            unbindRequested(ctx, e);
        }
        else {
            bindRequested(ctx, e);
        }
    }
    else if ( state == ChannelState::CONNECTED) {
        if (value.empty()) {
            disconnectRequested(ctx, e);
        }
        else {
            connectRequested(ctx, e);
        }
    }
    else if (state == ChannelState::INTEREST_OPS) {
        setInterestOpsRequested(ctx, e);
    }
    else {
        ctx.sendDownstream(e);
    }
}

void SimpleChannelDownstreamHandler::writeRequested(ChannelHandlerContext& ctx, const MessageEvent& e) {
    ctx.sendDownstream(e);
}

void SimpleChannelDownstreamHandler::bindRequested(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
    ctx.sendDownstream(e);
}

void SimpleChannelDownstreamHandler::connectRequested(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
    ctx.sendDownstream(e);
}

void SimpleChannelDownstreamHandler::setInterestOpsRequested(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
    ctx.sendDownstream(e);
}

void SimpleChannelDownstreamHandler::disconnectRequested(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
    ctx.sendDownstream(e);
}

void SimpleChannelDownstreamHandler::unbindRequested(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
    ctx.sendDownstream(e);
}

void SimpleChannelDownstreamHandler::closeRequested(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
    ctx.sendDownstream(e);
}

}}