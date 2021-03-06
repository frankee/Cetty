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

#include "cetty/channel/Channels.h"
#include "cetty/channel/ChannelHandlerContext.h"
#include "cetty/util/Integer.h"
#include "cetty/util/Exception.h"

#include "cetty/handler/codec/frame/TooLongFrameException.h"
#include "cetty/handler/codec/frame/DelimiterBasedFrameDecoder.h"

namespace cetty { namespace handler { namespace codec { namespace frame {

using namespace cetty::util;
using namespace cetty::channel;

DelimiterBasedFrameDecoder::DelimiterBasedFrameDecoder(
                                    int maxFrameLength,
                                    const ChannelBufferPtr& delimiter)
    : maxFrameLength(maxFrameLength),
      stripDelimiter(true),
      discardingTooLongFrame(false),
      tooLongFrameLength(0) {
    validateMaxFrameLength(maxFrameLength);
    validateDelimiter(delimiter);
    delimiters.push_back(delimiter->slice(delimiter->readerIndex(),
                                          delimiter->readableBytes()));
}

DelimiterBasedFrameDecoder::DelimiterBasedFrameDecoder(
                                    int maxFrameLength,
                                    bool stripDelimiter,
                                    const ChannelBufferPtr& delimiter)
    : maxFrameLength(maxFrameLength),
      stripDelimiter(stripDelimiter),
      discardingTooLongFrame(false),
      tooLongFrameLength(0) {
    validateMaxFrameLength(maxFrameLength);
    validateDelimiter(delimiter);
    delimiters.push_back(delimiter->slice(delimiter->readerIndex(),
                                          delimiter->readableBytes()));
}

DelimiterBasedFrameDecoder::DelimiterBasedFrameDecoder(
                                    int maxFrameLength,
                                    const std::vector<ChannelBufferPtr>& delimiters)
    : maxFrameLength(maxFrameLength),
      stripDelimiter(true),
      discardingTooLongFrame(false),
      tooLongFrameLength(0) {
    validateMaxFrameLength(maxFrameLength);
    initdelimiters(delimiters);
}

DelimiterBasedFrameDecoder::DelimiterBasedFrameDecoder(
                                    int maxFrameLength,
                                    bool stripDelimiter,
                                    const std::vector<ChannelBufferPtr>& delimiters)
    : maxFrameLength(maxFrameLength),
      stripDelimiter(stripDelimiter),
      discardingTooLongFrame(false),
      tooLongFrameLength(0) {
    validateMaxFrameLength(maxFrameLength);
    initdelimiters(delimiters);
}

cetty::channel::ChannelMessage
DelimiterBasedFrameDecoder::decode(ChannelHandlerContext& ctx,
                                   Channel& channel,
                                   const ChannelBufferPtr& buffer) {
    // Try all delimiters and choose the delimiter which yields the shortest frame.
    int minFrameLength = Integer::MAX_VALUE;
    ChannelBufferPtr minDelim;
    for (size_t i = 0; i < delimiters.size(); ++i) {
        int frameLength = indexOf(buffer, delimiters[i]);
        if (frameLength >= 0 && frameLength < minFrameLength) {
            minFrameLength = frameLength;
            minDelim = delimiters[i];
        }
    }

    if (minDelim) {
        int minDelimLength = minDelim->capacity();
        ChannelBufferPtr frame;

        if (discardingTooLongFrame) {
            // We've just finished discarding a very large frame.
            // Go back to the initial state.
            discardingTooLongFrame = false;
            buffer->skipBytes(minFrameLength + minDelimLength);

            // TODO Let user choose when the exception should be raised - early or late?
            //      If early, fail() should be called when discardingTooLongFrame is set to true.
            int tooLongFrameLength = this->tooLongFrameLength;
            this->tooLongFrameLength = 0;
            fail(ctx, tooLongFrameLength);
            return ChannelMessage::EMPTY_MESSAGE;
        }

        if (minFrameLength > maxFrameLength) {
            // Discard read frame.
            buffer->skipBytes(minFrameLength + minDelimLength);
            fail(ctx, minFrameLength);
            return ChannelMessage::EMPTY_MESSAGE;
        }

        if (stripDelimiter) {
            frame = buffer->readBytes(minFrameLength);
            buffer->skipBytes(minDelimLength);
        }
        else {
            frame = buffer->readBytes(minFrameLength + minDelimLength);
        }

        return ChannelMessage(frame);
    }
    else {
        if (!discardingTooLongFrame) {
            if (buffer->readableBytes() > maxFrameLength) {
                // Discard the content of the buffer until a delimiter is found.
                tooLongFrameLength = buffer->readableBytes();
                buffer->skipBytes(buffer->readableBytes());
                discardingTooLongFrame = true;
            }
        }
        else {
            // Still discarding the buffer since a delimiter is not found.
            tooLongFrameLength += buffer->readableBytes();
            buffer->skipBytes(buffer->readableBytes());
        }
        return ChannelMessage::EMPTY_MESSAGE;
    }
}

void DelimiterBasedFrameDecoder::fail(ChannelHandlerContext& ctx, long frameLength) {
    std::string msg;
    msg.reserve(64);

    if (frameLength > 0) {
        msg.append("frame length exceeds ");
        Integer::append(msg, maxFrameLength);
        msg.append(": ");
        Integer::append(msg, frameLength);
        msg.append(" - discarded");

        Channels::fireExceptionCaught(ctx.getChannel(),
                                      TooLongFrameException(msg));
    }
    else {
        msg.append("frame length exceeds ");
        Integer::append(msg, maxFrameLength);
        msg.append(" - discarded");

        Channels::fireExceptionCaught(ctx.getChannel(),
									  TooLongFrameException(msg));
    }
}

int DelimiterBasedFrameDecoder::indexOf(const ChannelBufferPtr& haystack,
                                        const ChannelBufferPtr& needle) {
    for (int i = haystack->readerIndex(); i < haystack->writerIndex(); ++i) {
        int haystackIndex = i;
        int needleIndex;
        for (needleIndex = 0; needleIndex < needle->capacity(); ++needleIndex) {
            if (haystack->getByte(haystackIndex) != needle->getByte(needleIndex)) {
                break;
            }
            else {
                haystackIndex ++;
                if (haystackIndex == haystack->writerIndex() &&
                    needleIndex != needle->capacity() - 1) {
                        return -1;
                }
            }
        }

        if (needleIndex == needle->capacity()) {
            // Found the needle from the haystack!
            return i - haystack->readerIndex();
        }
    }
    return -1;
}

void DelimiterBasedFrameDecoder::validateDelimiter(const ChannelBufferPtr& delimiter) {
    if (!delimiter) {
        throw NullPointerException("delimiter");
    }
    if (!delimiter->readable()) {
        throw InvalidArgumentException("empty delimiter");
    }
}

void DelimiterBasedFrameDecoder::validateMaxFrameLength(int maxFrameLength) {
    if (maxFrameLength <= 0) {
        throw InvalidArgumentException(
            std::string("maxFrameLength must be a positive integer: ") +
            Integer::toString(maxFrameLength));
    }
}

void DelimiterBasedFrameDecoder::initdelimiters(
                                const std::vector<ChannelBufferPtr>& delimiters) {
    if (delimiters.size() == 0) {
        throw InvalidArgumentException("empty delimiters");
    }

    for (size_t i = 0; i < delimiters.size(); ++i) {
        const ChannelBufferPtr& d = delimiters[i];
        validateDelimiter(d);
        this->delimiters.push_back(d->slice(d->readerIndex(), d->readableBytes()));
    } 
}

}}}}