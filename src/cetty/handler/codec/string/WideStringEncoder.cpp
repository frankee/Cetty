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

#include "cetty/handler/codec/string/WideStringEncoder.h"
#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/util/CharsetUtil.h"

namespace cetty { namespace handler { namespace codec { namespace string {

using namespace cetty::buffer;
using namespace cetty::util;

WideStringEncoder::WideStringEncoder() : charset(CharsetUtil::UTF_8) {
}

WideStringEncoder::WideStringEncoder(const Charset& charset) : charset(charset) {
}

ChannelMessage WideStringEncoder::encode(ChannelHandlerContext& ctx,
                                         Channel& channel,
                                         const ChannelMessage& msg) {
    if (msg.isWideString()) {
        return ChannelBuffers::copiedBuffer(msg.value<std::wstring>(), charset);
    }
    return msg;
}

}}}}