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

#include "cetty/handler/codec/protobuf/ProtobufDecoder.h"
#include "google/protobuf/message_lite.h"

namespace cetty { namespace handler { namespace codec { namespace protobuf {

using namespace cetty::channel;
using namespace google::protobuf;

ProtobufDecoder::ProtobufDecoder(MessageLite* prototype)
    : prototype(prototype) {
    BOOST_ASSERT(prototype);
}

cetty::channel::ChannelHandlerPtr ProtobufDecoder::clone() {
    return shared_from_this();
}

std::string ProtobufDecoder::toString() const {
    return "ProtobufDecoder";
}

cetty::channel::ChannelMessage ProtobufDecoder::decode(ChannelHandlerContext& ctx, Channel& channel, const ChannelMessage& msg) {
    if (!msg.isChannelBuffer()) {
        return msg;
    }

    ChannelBufferPtr& buf = msg.value<ChannelBufferPtr>();
    MessageLite* message = NULL;

    // TODO
    if (buf->hasArray()) {
        message = prototype->New();
    }
    else {
        message = prototype->New();
    }

    return ChannelMessage(message);
}

}}}}