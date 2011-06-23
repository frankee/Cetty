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

#include "cetty/channel/socket/asio/AsioWriteRequestQueue.h"
#include "cetty/buffer/CompositeChannelBuffer.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/channel/socket/asio/AsioSocketChannel.h"

namespace cetty { namespace channel { namespace socket { namespace asio {

using namespace cetty::channel;
using namespace cetty::buffer;

AsioWriteRequest::AsioWriteRequest(const MessageEvent& evt) {
    const ChannelMessage& message = evt.getMessage();
    if (message.isChannelBuffer()) {
        const ChannelBufferPtr& channelBuffer = message.value<ChannelBufferPtr>();
        if (channelBuffer->hasArray()) {
            Array array;
            channelBuffer->readSlice(array);

            writeBufferSize = array.length();
            buffer = asio_buffer(array.data(), writeBufferSize);
            gathring.buffers.truncatedIndex = 0;
        }
        else {
            channelBuffer->readSlice(gathring);
            writeBufferSize = gathring.bytesCount();

            if (gathring.buffers.truncatedIndex == 1) {
                buffer = gathring.buffers[0];
                gathring.buffers.truncatedIndex = 0;
            }
        }
    }
}

void AsioWriteOperationQueue::plusWriteBufferSize(int messageSize) {
    writeBufferSize += messageSize;
    
    if (NULL == channel) return;
    int highWaterMark = channel->config.getWriteBufferHighWaterMark();

    if (writeBufferSize >= highWaterMark) {
        if (writeBufferSize - messageSize < highWaterMark) {
            channel->handleAtHighWaterMark();
        }
    }
}

void AsioWriteOperationQueue::minusWriteBufferSize(int messageSize) {
    writeBufferSize -= messageSize;

    if (NULL == channel) return;
    int lowWaterMark = channel->config.getWriteBufferLowWaterMark();

    if (writeBufferSize == 0 || writeBufferSize < lowWaterMark) {
        if (writeBufferSize + messageSize >= lowWaterMark) {
            channel->handleAtLowWaterMark();
        }
    }
}

}}}}