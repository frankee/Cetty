#if !defined(CETTY_BUFFER_READONLYCHANNELBUFFER_H)
#define CETTY_BUFFER_READONLYCHANNELBUFFER_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include "cetty/buffer/WrappedChannelBuffer.h"
#include "cetty/buffer/AbstractChannelBuffer.h"
#include "cetty/buffer/ReadOnlyBufferException.h"

namespace cetty { namespace buffer {

using namespace cetty::util;

/**
 * A derived buffer which forbids any write requests to its parent.  It is
 * recommended to use {@link ChannelBuffers#unmodifiableBuffer(ChannelBuffer)}
 * instead of calling the constructor explicitly.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class ReadOnlyChannelBuffer : public AbstractChannelBuffer, public WrappedChannelBuffer {
public:
    ReadOnlyChannelBuffer(const ChannelBufferPtr& buffer) : buffer(buffer) {
        BOOST_ASSERT(buffer);
        AbstractChannelBuffer::setIndex(buffer->readerIndex(), buffer->writerIndex());
    }

    virtual ~ReadOnlyChannelBuffer() {}

    virtual ChannelBufferPtr& unwrap() {
        return buffer;
    }

    virtual ChannelBufferFactory& factory() const {
        return buffer->factory();
    }

    virtual ByteOrder order() const {
        return buffer->order();
    }

    virtual void readableBytes(Array& array) {}
    virtual void writableBytes(Array& array) {}

    virtual bool hasArray() const {
        return false;
    }

    virtual const Array& array() {
        throw ReadOnlyBufferException();
    }

    virtual ConstArray array() const {
        throw ReadOnlyBufferException();
    }

    virtual int arrayOffset() const {
        throw ReadOnlyBufferException();
    }

    virtual void discardReadBytes() {
        throw ReadOnlyBufferException();
    }

    virtual void setByte(int index, int value) {
        throw ReadOnlyBufferException();
    }

    virtual void setBytes(int index, const ChannelBuffer& src, int srcIndex, int length) {
        throw ReadOnlyBufferException();
    }

    virtual void setBytes(int index, const ConstArray& src, int srcIndex, int length) {
        throw ReadOnlyBufferException();
    }

    virtual void setShort(int index, int value) {
        throw ReadOnlyBufferException();
    }

    virtual void setMedium(int index, int value) {
        throw ReadOnlyBufferException();
    }

    virtual void setInt(int index, int value) {
        throw ReadOnlyBufferException();
    }

    virtual void setLong(int index, boost::int64_t value) {
        throw ReadOnlyBufferException();
    }

    virtual int setBytes(int index, InputStream& in, int length) {
        throw ReadOnlyBufferException();
    }

    virtual void getBytes(int index, OutputStream& out, int length) const {
        buffer->getBytes(index, out, length);
    }

    virtual void getBytes(int index, const Array& dst, int dstIndex, int length) const {
        buffer->getBytes(index, dst, dstIndex, length);
    }

    virtual void getBytes(int index, ChannelBuffer& dst, int dstIndex, int length) const {
        buffer->getBytes(index, dst, dstIndex, length);
    }

    virtual ChannelBufferPtr duplicate() {
        return ChannelBufferPtr(new ReadOnlyChannelBuffer(*this));
    }

    virtual ChannelBufferPtr copy(int index, int length) const {
        return buffer->copy(index, length);
    }

    virtual ChannelBufferPtr slice(int index, int length) {
        ChannelBufferPtr buf = buffer->slice(index, length);
        return ChannelBufferPtr(new ReadOnlyChannelBuffer(buf));
    }

    virtual void slice(Array& array) {}

    virtual boost::int8_t getByte(int index) const {
        return buffer->getByte(index);
    }

    virtual boost::int16_t getShort(int index) const {
        return buffer->getShort(index);
    }

    virtual boost::int32_t getUnsignedMedium(int index) const {
        return buffer->getUnsignedMedium(index);
    }

    virtual boost::int32_t getInt(int index) const {
        return buffer->getInt(index);
    }

    virtual boost::int64_t getLong(int index) const {
        return buffer->getLong(index);
    }

    virtual int capacity() const {
        return buffer->capacity();
    }

    virtual void readSlice(Array& array){}
    virtual void readSlice(GatheringBuffer& gathering) {}

protected:
    ReadOnlyChannelBuffer(const ReadOnlyChannelBuffer& buffer) : buffer(buffer.buffer) {
        AbstractChannelBuffer::setIndex(this->buffer->readerIndex(), this->buffer->writerIndex());
    }

private:
    ChannelBufferPtr buffer;
};

typedef boost::intrusive_ptr<ReadOnlyChannelBuffer> ReadOnlyChannelBufferPtr;

}}

#endif //#if !defined(CETTY_BUFFER_READONLYCHANNELBUFFER_H)
