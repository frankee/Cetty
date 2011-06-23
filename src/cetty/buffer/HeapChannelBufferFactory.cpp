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

#include "cetty/buffer/HeapChannelBufferFactory.h"
#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/util/Exception.h"

namespace cetty { namespace buffer {

using namespace cetty::util;

HeapChannelBufferFactory HeapChannelBufferFactory::INSTANCE_BE = HeapChannelBufferFactory(ByteOrder::BYTE_ORDER_BIG);
HeapChannelBufferFactory HeapChannelBufferFactory::INSTANCE_LE = HeapChannelBufferFactory(ByteOrder::BYTE_ORDER_LITTLE);

ChannelBufferFactory& HeapChannelBufferFactory::getInstance() {
    return INSTANCE_BE;
}

ChannelBufferFactory& HeapChannelBufferFactory::getInstance(ByteOrder endianness) {
    if (endianness == ByteOrder::BYTE_ORDER_BIG) {
        return INSTANCE_BE;
    }
    else if (endianness == ByteOrder::BYTE_ORDER_LITTLE) {
        return INSTANCE_LE;
    }
    else {
        throw IllegalStateException("Should not reach here");
    }
}

cetty::buffer::ChannelBufferPtr HeapChannelBufferFactory::getBuffer(ByteOrder order, int capacity) {
    return ChannelBuffers::buffer(order, capacity);
}

cetty::buffer::ChannelBufferPtr HeapChannelBufferFactory::getBuffer(ByteOrder order, const Array& array, int offset, int length) {
    return ChannelBuffers::wrappedBuffer(order, array, offset, length);
}

}}