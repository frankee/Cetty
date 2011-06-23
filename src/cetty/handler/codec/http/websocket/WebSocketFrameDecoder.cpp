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

#include "cetty/handler/codec/http/websocket/WebSocketFrameDecoder.h"
#include "cetty/handler/codec/frame/TooLongFrameException.h"
#include "cetty/handler/codec/replay/ReplayingDecoderBuffer.h"
#include "cetty/handler/codec/http/websocket/DefaultWebSocketFrame.h"

namespace cetty { namespace handler { namespace codec { namespace http { namespace websocket {

using namespace cetty::handler::codec::frame;
using namespace cetty::handler::codec::replay;

ChannelMessage WebSocketFrameDecoder::decode(ChannelHandlerContext& ctx,
                                             Channel& channel,
                                             const ReplayingDecoderBufferPtr& buffer,
                                             int state) {
    // Discard all data received if closing handshake was received before.
    if (receivedClosingHandshake) {
        buffer->skipBytes(actualReadableBytes());
        return ChannelMessage::EMPTY_MESSAGE;
    }

    // Decode a frame otherwise.
    boost::int8_t type = buffer->readByte();
    if (buffer->needMoreBytes()) {
        return ChannelMessage::EMPTY_MESSAGE;
    }

    if ((type & 0x80) == 0x80) {
        // If the MSB on type is set, decode the frame length
        return decodeBinaryFrame(type, buffer);
    }
    else {
        // Decode a 0xff terminated UTF-8 string
        return decodeTextFrame(type, buffer);
    }
}

ChannelMessage WebSocketFrameDecoder::decodeBinaryFrame(int type,
                                                        const ReplayingDecoderBufferPtr& buffer) {
    int frameSize = 0;
    int lengthFieldSize = 0;
    boost::int8_t b;
    do {
        b = buffer->readByte();
        if (buffer->needMoreBytes()) {
            return ChannelMessage::EMPTY_MESSAGE;
        }

        frameSize <<= 7;
        frameSize |= b & 0x7f;
        if (frameSize > maxFrameSize) {
            throw TooLongFrameException();
        }
        ++lengthFieldSize;
        if (lengthFieldSize > 8) {
            // Perhaps a malicious peer?
            throw TooLongFrameException();
        }
    } while ((b & 0x80) == 0x80);

    if (type == 0xFF && frameSize == 0) {
        receivedClosingHandshake = true;
    }

    return ChannelMessage(WebSocketFramePtr(
        new DefaultWebSocketFrame(type, buffer->readBytes(frameSize))));
}

ChannelMessage WebSocketFrameDecoder::decodeTextFrame(int type,
                                                      const ReplayingDecoderBufferPtr& buffer) {
    int ridx = buffer->readerIndex();
    int rbytes = actualReadableBytes();
    int delimPos = buffer->indexOf(ridx, ridx + rbytes, (boost::int8_t)0xFF);
    if (delimPos == -1) {
        // Frame delimiter (0xFF) not found
        if (rbytes > maxFrameSize) {
            // Frame length exceeded the maximum
            throw TooLongFrameException();
        }
        else {
            // Wait until more data is received
            return ChannelMessage::EMPTY_MESSAGE;
        }
    }

    int frameSize = delimPos - ridx;
    if (frameSize > maxFrameSize) {
        throw TooLongFrameException();
    }

    ChannelBufferPtr binaryData = buffer->readBytes(frameSize);
    buffer->skipBytes(1);

    return ChannelMessage(
        WebSocketFramePtr(new DefaultWebSocketFrame(type, binaryData)));
}

}}}}}
