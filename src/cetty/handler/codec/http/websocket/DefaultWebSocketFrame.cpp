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

#include "cetty/handler/codec/http/websocket/DefaultWebSocketFrame.h"

#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/util/Integer.h"

namespace cetty { namespace handler { namespace codec { namespace http { namespace websocket {

using namespace cetty::buffer;
using namespace cetty::util;

DefaultWebSocketFrame::DefaultWebSocketFrame() {
    setData(0, ChannelBuffers::EMPTY_BUFFER);
}

DefaultWebSocketFrame::DefaultWebSocketFrame(const std::string& textData) {
    setData(0, ChannelBuffers::copiedBuffer(textData));
}

DefaultWebSocketFrame::DefaultWebSocketFrame(const Array& textData) {
    setData(0, ChannelBuffers::wrappedBuffer(textData));
}

DefaultWebSocketFrame::DefaultWebSocketFrame(int type, const ChannelBufferPtr& binaryData) {
    setData(type, binaryData);
}

const char* DefaultWebSocketFrame::getTextData() const {
    // if binaryData has capacity, just pad a '\0', then return the c string
    if (binaryData->writable()) {
        binaryData->writeByte('\0');
        return binaryData->array().data();
    }
    else {
        text.clear();
        binaryData->readBytes(text);
        return text.c_str();
    }
}

void DefaultWebSocketFrame::setData(int type, const ChannelBufferPtr& binaryData) {
    if (!binaryData) {
        throw NullPointerException("binaryData");
    }

    if ((type & 0x80) == 0) {
        // If text, data should not contain 0xFF.
        int delimPos = binaryData->indexOf(binaryData->readerIndex(),
            binaryData->writerIndex(),
            (boost::uint8_t)0xFF);
        if (delimPos >= 0) {
            throw InvalidArgumentException(
                "a text frame should not contain 0xFF.");
        }
    }

    this->type = type & 0xFF;
    this->binaryData = binaryData;
}

std::string DefaultWebSocketFrame::toString() const {
    std::string str("DefaultWebSocketFrame");
    str.append("(type: ");
    str.append(Integer::toString(getType()));

    if (binaryData) {
        str.append(", data: ");
        str.append(ChannelBuffers::hexDump(*binaryData));
    }
    else {
        str.append(", data is EMPTY");
    }

    str.append(")");

    return str;
}

}}}}}