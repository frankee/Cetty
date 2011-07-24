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

#include "cetty/handler/codec/protobuf/ProtobufEncoder.h"
#include "google/protobuf/message_lite.h"

namespace cetty { namespace handler { namespace codec { namespace protobuf {

using namespace google::protobuf;
using namespace cetty::channel;

ChannelHandlerPtr ProtobufEncoder::clone() {
    return shared_from_this();
}

std::string ProtobufEncoder::toString() const {
    return "ProtobufEncoder";
}

ChannelMessage ProtobufEncoder::encode(ChannelHandlerContext& ctx,
                                       Channel& channel,
                                       const ChannelMessage& msg) {
    MessageLite* message = msg.rawPointer<MessageLite>();
    if (NULL == message) {
        return msg;
    }

    int size = message->GetCachedSize();
    ChannelBufferPtr newBuffer = ChannelBuffers::buffer(size);
    Array array = newBuffer->array();
    message->SerializeWithCachedSizesToArray((boost::uint8_t*)array.data());
    return ChannelMessage(newBuffer);
}

}}}}