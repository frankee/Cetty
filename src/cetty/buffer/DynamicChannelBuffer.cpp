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

#include "cetty/buffer/DynamicChannelBuffer.h"
#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/buffer/DuplicatedChannelBuffer.h"
#include "cetty/buffer/TruncatedChannelBuffer.h"
#include "cetty/buffer/SlicedChannelBuffer.h"
#include "cetty/buffer/HeapChannelBufferFactory.h"

#include "cetty/util/Exception.h"

namespace cetty { namespace buffer {

using namespace cetty::util;

DynamicChannelBuffer::DynamicChannelBuffer(int estimatedLength)
    : endianness(ByteOrder::BYTE_ORDER_BIG),
      bufferFactory(HeapChannelBufferFactory::getInstance(ByteOrder::BYTE_ORDER_BIG)) {
    if (estimatedLength < 0) {
        throw InvalidArgumentException("estimatedLength is negtive: " /*+ estimatedLength*/);
    }
    buffer = bufferFactory.getBuffer(endianness, estimatedLength);
}

DynamicChannelBuffer::DynamicChannelBuffer(ByteOrder endianness, int estimatedLength)
    : endianness(endianness),
      bufferFactory(HeapChannelBufferFactory::getInstance(endianness)) {
    if (estimatedLength < 0) {
        throw InvalidArgumentException("estimatedLength is negative.");
    }
    buffer = bufferFactory.getBuffer(endianness, estimatedLength);
}

DynamicChannelBuffer::DynamicChannelBuffer(ByteOrder endianness, int estimatedLength, ChannelBufferFactory& factory)
    : endianness(endianness), bufferFactory(factory) {
    if (estimatedLength < 0) {
        throw InvalidArgumentException("estimatedLength is negative: " );
    }

    buffer = bufferFactory.getBuffer(endianness, estimatedLength);
}

void DynamicChannelBuffer::ensureWritableBytes(int minWritableBytes) {
    if (minWritableBytes <= writableBytes()) {
        return;
    }

    int newCapacity;
    if (capacity() == 0) {
        newCapacity = 1;
    }
    else {
        newCapacity = capacity();
    }
    int minNewCapacity = writerIndex() + minWritableBytes;
    while (newCapacity < minNewCapacity) {
        newCapacity <<= 1;
    }

    ChannelBufferPtr newBuffer = factory().getBuffer(order(), newCapacity);
    newBuffer->writeBytes(*buffer, 0, writerIndex());
    buffer.swap(newBuffer);
}

cetty::buffer::ChannelBufferPtr DynamicChannelBuffer::duplicate() {
    return ChannelBufferPtr(new DuplicatedChannelBuffer(shared_from_this()));
}

cetty::buffer::ChannelBufferPtr DynamicChannelBuffer::copy(int index, int length) const {
    DynamicChannelBuffer* copiedBuffer =
        new DynamicChannelBuffer(order(), length > 64 ? length : 64, factory());

    copiedBuffer->buffer = buffer->copy(index, length);
    copiedBuffer->setIndex(0, length);
    return ChannelBufferPtr(copiedBuffer);
}

cetty::buffer::ChannelBufferPtr DynamicChannelBuffer::slice(int index, int length) {
    if (index == 0) {
        if (length == 0) {
            return ChannelBuffers::EMPTY_BUFFER;
        }
        return ChannelBufferPtr(new TruncatedChannelBuffer(shared_from_this(), length));
    }
    else {
        if (length == 0) {
            return ChannelBuffers::EMPTY_BUFFER;
        }
        return ChannelBufferPtr(new SlicedChannelBuffer(shared_from_this(), index, length));
    }
}

void DynamicChannelBuffer::readSlice(Array& array) {
    array.reset(buffer->array().data(readerIdx), writerIdx - readerIdx);
    readerIdx = writerIdx = 0;
}

void DynamicChannelBuffer::readSlice(GatheringBuffer& gathering) {
}

}}
