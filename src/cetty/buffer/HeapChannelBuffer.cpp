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

#include "cetty/buffer/HeapChannelBuffer.h"

#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/buffer/TruncatedChannelBuffer.h"
#include "cetty/buffer/SlicedChannelBuffer.h"
#include "cetty/buffer/GatheringBuffer.h"

#include "cetty/util/InputStream.h"
#include "cetty/util/OutputStream.h"
#include "cetty/util/Exception.h"

namespace cetty { namespace buffer {

using namespace cetty::util;

HeapChannelBuffer::HeapChannelBuffer(int length)
    : maintainArrayBuffer(true) {
    if (length < 0) {
        throw InvalidArgumentException("length must greater than 0.");
    }

    if (length > 0) {
        arry = Array(new char[length], length);
    }
    else {
        arry = Array(new char[1], 0);
    }
}

HeapChannelBuffer::HeapChannelBuffer(char* buf, int length)
    : AbstractChannelBuffer(0, length), maintainArrayBuffer(false), arry(buf, length) {
}

HeapChannelBuffer::HeapChannelBuffer(const Array& array)
    : AbstractChannelBuffer(0, array.length()), maintainArrayBuffer(false), arry(array) {
}

HeapChannelBuffer::HeapChannelBuffer(const Array& array, bool maintainedBuf)
    : AbstractChannelBuffer(0, array.length()),
      maintainArrayBuffer(maintainedBuf),
      arry(array) {
}

HeapChannelBuffer::~HeapChannelBuffer() {
    if (maintainArrayBuffer) {
        delete[] arry.data();
    }
}

int HeapChannelBuffer::capacity() const {
    return this->arry.length();
}

bool HeapChannelBuffer::hasArray() const {
    return true;
}

void HeapChannelBuffer::offsetWriterIndex(int offset) {
    int newWriterIdx = this->writerIdx + offset;
    if (newWriterIdx < readerIdx || newWriterIdx > this->arry.length()) {
        throw RangeException("writerIndex");
    }
    this->writerIdx = newWriterIdx;
}

void HeapChannelBuffer::readableBytes(Array& array) {
    array.reset(this->arry.data(readerIdx), writerIdx - readerIdx);
}

void HeapChannelBuffer::writableBytes(Array& array) {
    array.reset(this->arry.data(writerIdx), this->arry.length() - readerIdx);
}

boost::int8_t HeapChannelBuffer::getByte(int index) const {
    return this->arry[index];
}

void HeapChannelBuffer::getBytes(int index, ChannelBuffer& dst, int dstIndex, int length) const {
    HeapChannelBuffer* dstBuffer = dynamic_cast<HeapChannelBuffer*>(&dst);

    if (dstBuffer) {
        getBytes(index, dstBuffer->arry, dstIndex, length);
    }
    else {
        dst.setBytes(dstIndex, this->arry, index, length);
    }
}

void HeapChannelBuffer::getBytes(int index, const Array& dst, int dstIndex, int length) const {
    Array::copy(dst, dstIndex, this->arry, index, length);
}

void HeapChannelBuffer::getBytes(int index, OutputStream& out, int length) const {
    out.write((const boost::int8_t*)arry.data(), index, length);
}

void HeapChannelBuffer::setByte(int index, int value) {
    this->arry[index] = (char)value;
}

void HeapChannelBuffer::setBytes(int index, const ChannelBuffer& src, int srcIndex, int length) {
    const HeapChannelBuffer* heap =
        dynamic_cast<const HeapChannelBuffer*>(&src);

    if (heap) {
        setBytes(index, heap->arry, srcIndex, length);
    }
    else {
        src.getBytes(srcIndex, this->arry, index, length);
    }
}

void HeapChannelBuffer::setBytes(int index, const ConstArray& src, int srcIndex, int length) {
    Array::copy(this->arry, index, src, srcIndex, length);
}

int HeapChannelBuffer::setBytes(int index, InputStream& in, int length) {
    int readBytes = 0;
    do {
        int localReadBytes = in.read((boost::int8_t*)arry.data(), index, length);
        if (localReadBytes < 0) {
            if (readBytes == 0) {
                return -1;
            } else {
                break;
            }
        }
        readBytes += localReadBytes;
        index += localReadBytes;
        length -= localReadBytes;
    } while (length > 0);

    return readBytes;
}

cetty::buffer::ChannelBufferPtr HeapChannelBuffer::slice(int index, int length) {
    if (index == 0) {
        if (length == 0) {
            return ChannelBuffers::EMPTY_BUFFER;
        }
        if (length == this->arry.length()) {
            ChannelBufferPtr slice = duplicate();
            slice->setIndex(0, length);
            return slice;
        }
        else {
            return ChannelBufferPtr(
                new TruncatedChannelBuffer(shared_from_this(), length));
        }
    }
    else {
        if (length == 0) {
            return ChannelBuffers::EMPTY_BUFFER;
        }
        return ChannelBufferPtr(new SlicedChannelBuffer(shared_from_this(), index, length));
    }
}

void HeapChannelBuffer::slice(Array& array) {
    array.reset(this->arry.data(readerIdx), writerIdx - readerIdx);
}

void HeapChannelBuffer::readSlice(Array& array) {
    array.reset(this->arry.data(readerIdx), writerIdx - readerIdx);
    readerIdx = writerIdx = 0;
}

void HeapChannelBuffer::readSlice(GatheringBuffer& gatheringBuffer) {
    gatheringBuffer.append(array().data(readerIdx), writerIdx - readerIdx);
    readerIdx = writerIdx = 0;
}

}}
