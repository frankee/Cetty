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

#include <typeinfo>

#include "cetty/handler/codec/replay/ReplayingDecoder.h"
#include "cetty/handler/codec/replay/UnsafeDynamicChannelBuffer.h"

#include "cetty/buffer/ChannelBuffer.h"
#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/channel/Channel.h"
#include "cetty/channel/Channels.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/channel/ChannelConfig.h"
#include "cetty/channel/ChannelHandlerContext.h"
#include "cetty/channel/SimpleChannelUpstreamHandler.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ExceptionEvent.h"
#include "cetty/channel/ChannelStateEvent.h"
#include "cetty/channel/WriteCompletionEvent.h"
#include "cetty/channel/ChildChannelStateEvent.h"
#include "cetty/channel/SocketAddress.h"

#include "cetty/util/Exception.h"

namespace cetty { namespace handler { namespace codec { namespace replay { 


void ReplayingDecoder::messageReceived(ChannelHandlerContext& ctx,
                                       const MessageEvent& e) {
    const ChannelMessage& m = e.getMessage();
    ChannelBufferPtr input = m.smartPointer<ChannelBuffer>();
    if (!input) {
        ctx.sendUpstream(e);
        return;
    }

    if (!input->readable()) {
        return;
    }

    callDecode(ctx,
               e.getChannel(),
               getCumulation(ctx, input),
               e.getRemoteAddress());
}

void ReplayingDecoder::channelDisconnected(ChannelHandlerContext& ctx,
                                           const ChannelStateEvent& e) {
    cleanup(ctx, e);
}

void ReplayingDecoder::channelClosed(ChannelHandlerContext& ctx,
                                     const ChannelStateEvent& e) {
    cleanup(ctx, e);
}

void ReplayingDecoder::exceptionCaught(ChannelHandlerContext& ctx,
                                       const ExceptionEvent& e) {
    ctx.sendUpstream(e);
}

const ChannelBufferPtr& ReplayingDecoder::internalBuffer() const {
    if (!cumulation) {
        return ChannelBuffers::EMPTY_BUFFER;
    }
    return cumulation;
}

void ReplayingDecoder::callDecode(ChannelHandlerContext& context, Channel& channel, const ChannelBufferPtr& cumulation, const SocketAddress& remoteAddress) {
    while (cumulation->readable()) {
        int oldReaderIndex = checkedPoint = cumulation->readerIndex();
        ChannelMessage result;
        int oldState = state;

        // 1, discard
        // 2, need more
        // 3, ok
        result = decode(context, channel, replayable, state);
        if (result.empty()) {
            if (replayable->needMoreBytes()) {
                // Return to the checkpoint (or oldPosition) and retry.
                if (checkedPoint >= 0) {
                    cumulation->readerIndex(checkedPoint);
                }
                else {
                    // Called by cleanup() - no need to maintain the readerIndex
                    // anymore because the buffer has been released already.
                }

                // Seems like more data is required.
                // Let us wait for the next notification.
                break;
            }
            else if (oldReaderIndex == cumulation->readerIndex() && oldState == state) {
                throw IllegalStateException(
                    "null cannot be returned if no data is consumed and state didn't change.");
            }
            else {
                // Previous data has been discarded or caused state transition.
                // Probably it is reading on.
                continue;
            }
        }

        if (oldReaderIndex == cumulation->readerIndex() && oldState == state) {
            throw IllegalStateException(
                std::string("decode() method must consume at least one byte \
                            if it returned a decoded message (caused by: ") +
                            typeid(*this).name() +
                            std::string(")"));
        }

        // A successful decode
        unfoldAndfireMessageReceived(context, result, remoteAddress);
    }
}

void ReplayingDecoder::unfoldAndfireMessageReceived(ChannelHandlerContext& context, ChannelMessage& result, const SocketAddress& remoteAddress) {
    if (unfold) {
        if (result.isVector()) {
            int size = result.vectorSize();
            for (int i = 0; i < size; ++i) {
                Channels::fireMessageReceived(context,
                                              result.value<ChannelMessage>(i),
                                              remoteAddress);
            }
        }
        else {
            Channels::fireMessageReceived(context, result, remoteAddress);
        }
    }
    else {
        Channels::fireMessageReceived(context, result, remoteAddress);
    }
}

void ReplayingDecoder::cleanup(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
    ChannelBufferPtr cumulation = this->cumulation;
    this->cumulation.reset();

    if (!cumulation) {
        return;
    }

    replayable->terminate();
    if (cumulation->readable()) {
        // Make sure all data was read before notifying a closed channel.
        callDecode(ctx, e.getChannel(), cumulation, SocketAddress::NULL_ADDRESS);
    }

    // Call decodeLast() finally.  Please note that decodeLast() is
    // called even if there's nothing more to read from the buffer to
    // notify a user that the connection was closed explicitly.
    ChannelMessage partiallyDecoded = decodeLast(ctx, e.getChannel(), replayable, state);
    if (!partiallyDecoded.empty()) {
        unfoldAndfireMessageReceived(ctx, partiallyDecoded, SocketAddress::NULL_ADDRESS);
    }

    ctx.sendUpstream(e);
}

const ChannelBufferPtr& ReplayingDecoder::getCumulation(ChannelHandlerContext& ctx, const ChannelBufferPtr& input) {
    channelOwnBuffer = ctx.getChannel().getConfig().channelOwnBuffer();

    if (!cumulation) {
        if (channelOwnBuffer) {
            cumulation = input;
        }
        else {
            ChannelBufferFactory* factory = ctx.getChannel().getConfig().getBufferFactory();
            BOOST_ASSERT(factory);

            cumulation = ChannelBufferPtr(new UnsafeDynamicChannelBuffer(*factory));
        }

        replayable = ReplayingDecoderBufferPtr(new ReplayingDecoderBuffer(cumulation));
    }

    // input buffer changes (may re allocate memory) which channel owns buffer
    if (channelOwnBuffer && !cumulation->equals(*input)) {
        cumulation = input;
        replayable = ReplayingDecoderBufferPtr(new ReplayingDecoderBuffer(input));
    }

    if (!channelOwnBuffer) {
        cumulation->discardReadBytes();
        cumulation->writeBytes(*input);
    }

    return cumulation;
}

}}}}