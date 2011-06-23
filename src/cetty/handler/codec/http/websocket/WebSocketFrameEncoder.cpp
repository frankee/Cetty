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

#include "cetty/handler/codec/http/websocket/WebSocketFrameEncoder.h"
#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelConfig.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/buffer/ChannelBuffer.h"
#include "cetty/buffer/ChannelBufferFactory.h"
#include "cetty/handler/codec/http/websocket/WebSocketFrame.h"

namespace cetty { namespace handler { namespace codec { namespace http { namespace websocket {

using namespace cetty::channel;
using namespace cetty::buffer;

cetty::channel::ChannelMessage WebSocketFrameEncoder::encode(ChannelHandlerContext& ctx,
                                                             Channel& channel,
                                                             const ChannelMessage& msg) {
    WebSocketFrame* frame = msg.rawPointer<WebSocketFrame>();
    if (frame) {
        int type = frame->getType();
        if (frame->isText()) {
            // Text frame
            const ChannelBufferPtr& data = frame->getBinaryData();
            int dataLen = data->readableBytes();
            ChannelBufferPtr encoded =
                channel.getConfig().getBufferFactory()->getBuffer(data->order(), dataLen + 2);

            encoded->writeByte(type);
            encoded->writeBytes(*data, data->readerIndex(), dataLen);
            encoded->writeByte(0xFF);
            return encoded;
        }
        else {
            // Binary frame
            const ChannelBufferPtr& data = frame->getBinaryData();
            int dataLen = data->readableBytes();
            ChannelBufferPtr encoded =
                channel.getConfig().getBufferFactory()->getBuffer(data->order(), dataLen + 5);

            // Encode type.
            encoded->writeByte(type);

            // Encode length.
            int b1 = (dataLen >> 28) & 0x7F;
            int b2 = (dataLen >> 14) & 0x7F;
            int b3 = (dataLen >>  7) & 0x7F;
            int b4 = dataLen & 0x7F;
            if (b1 == 0) {
                if (b2 == 0) {
                    if (b3 == 0) {
                        encoded->writeByte(b4);
                    }
                    else {
                        encoded->writeByte(b3 | 0x80);
                        encoded->writeByte(b4);
                    }
                }
                else {
                    encoded->writeByte(b2 | 0x80);
                    encoded->writeByte(b3 | 0x80);
                    encoded->writeByte(b4);
                }
            }
            else {
                encoded->writeByte(b1 | 0x80);
                encoded->writeByte(b2 | 0x80);
                encoded->writeByte(b3 | 0x80);
                encoded->writeByte(b4);
            }

            // Encode binary data.
            encoded->writeBytes(*data, data->readerIndex(), dataLen);
            return encoded;
        }
    }
    return msg;
}

}}}}}
