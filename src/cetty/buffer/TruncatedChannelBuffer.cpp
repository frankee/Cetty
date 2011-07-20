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

#include "cetty/buffer/TruncatedChannelBuffer.h"
#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/util/Integer.h"
#include "cetty/util/Exception.h"

namespace cetty { namespace buffer {

using namespace cetty::util;

TruncatedChannelBuffer::TruncatedChannelBuffer(const ChannelBufferPtr& buffer,
                                               int length)
    : buffer(buffer), length(length) {
    if (this->length > this->buffer->capacity()) {
        throw RangeException("");
    }

    writerIndex(length);
}

void TruncatedChannelBuffer::readableBytes(Array& array) {
    array.reset(this->buffer->array().data(readerIdx), writerIdx - readerIdx);
}

void TruncatedChannelBuffer::writableBytes(Array& array) {
}

boost::int8_t TruncatedChannelBuffer::getByte(int index) const {
    checkIndex(index);
    return buffer->getByte(index);
}

boost::int16_t TruncatedChannelBuffer::getShort(int index) const {
    checkIndex(index, 2);
    return buffer->getShort(index);
}

boost::int32_t TruncatedChannelBuffer::getUnsignedMedium(int index) const {
    checkIndex(index, 3);
    return buffer->getUnsignedMedium(index);
}

boost::int32_t TruncatedChannelBuffer::getInt(int index) const {
    checkIndex(index, 4);
    return buffer->getInt(index);
}

boost::int64_t TruncatedChannelBuffer::getLong(int index) const {
    checkIndex(index, 8);
    return buffer->getLong(index);
}

cetty::buffer::ChannelBufferPtr TruncatedChannelBuffer::duplicate() {
    ChannelBufferPtr duplicateBuff = 
        ChannelBufferPtr(new TruncatedChannelBuffer(buffer, length));
    duplicateBuff->setIndex(readerIndex(), writerIndex());
    return duplicateBuff;
}

cetty::buffer::ChannelBufferPtr TruncatedChannelBuffer::copy(int index, int length) const {
    checkIndex(index, length);
    return buffer->copy(index, length);
}

cetty::buffer::ChannelBufferPtr TruncatedChannelBuffer::slice(int index, int length) {
    checkIndex(index, length);
    if (length == 0) {
        return ChannelBuffers::EMPTY_BUFFER;
    }
    return buffer->slice(index, length);
}

void TruncatedChannelBuffer::slice(Array& array) {
    if (length > 0) {
        array.reset(this->buffer->array().data(readerIdx), writerIdx - readerIdx);
    }
}

void TruncatedChannelBuffer::getBytes(int index, ChannelBuffer& dst, int dstIndex, int length) const {
    checkIndex(index, length);
    buffer->getBytes(index, dst, dstIndex, length);
}

void TruncatedChannelBuffer::getBytes(int index, const Array& dst, int dstIndex, int length) const {
    checkIndex(index, length);
    buffer->getBytes(index, dst, dstIndex, length);
}

void TruncatedChannelBuffer::getBytes(int index, OutputStream& out, int length) const {
    checkIndex(index, length);
    buffer->getBytes(index, out, length);
}

void TruncatedChannelBuffer::setByte(int index, int value) {
    checkIndex(index);
    buffer->setByte(index, value);
}

void TruncatedChannelBuffer::setShort(int index, int value) {
    checkIndex(index, 2);
    buffer->setShort(index, value);
}

void TruncatedChannelBuffer::setMedium(int index, int value) {
    checkIndex(index, 3);
    buffer->setMedium(index, value);
}

void TruncatedChannelBuffer::setInt(int index, int value) {
    checkIndex(index, 4);
    buffer->setInt(index, value);
}

void TruncatedChannelBuffer::setLong(int index, boost::int64_t value) {
    checkIndex(index, 8);
    buffer->setLong(index, value);
}

void TruncatedChannelBuffer::setBytes(int index, const ConstArray& src, int srcIndex, int length) {
    checkIndex(index, length);
    buffer->setBytes(index, src, srcIndex, length);
}

void TruncatedChannelBuffer::setBytes(int index, const ChannelBuffer& src, int srcIndex, int length) {
    checkIndex(index, length);
    buffer->setBytes(index, src, srcIndex, length);
}

int TruncatedChannelBuffer::setBytes(int index, InputStream& in, int length) {
    checkIndex(index, length);
    return buffer->setBytes(index, in, length);
}

void TruncatedChannelBuffer::checkIndex(int index) const {
    if (index < 0 || index >= capacity()) {
        throw RangeException("");
    }
}

void TruncatedChannelBuffer::checkIndex(int index, int length) const {
    if (length < 0) {
        throw InvalidArgumentException(
            std::string("length is negative: ") + Integer::toString(length));
    }
    if (index + length > this->length) {
        throw RangeException("");
    }
}

void TruncatedChannelBuffer::readSlice(Array& array) {
    array.reset(buffer->array().data(readerIdx), writerIdx - readerIdx);
    readerIdx = writerIdx = 0;
}

void TruncatedChannelBuffer::readSlice(GatheringBuffer& gathering) {

}

}}