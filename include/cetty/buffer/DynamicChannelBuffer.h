#if !defined(CETTY_BUFFER_DYNAMICCHANNELBUFFER_H)
#define CETTY_BUFFER_DYNAMICCHANNELBUFFER_H

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

#include "cetty/buffer/AbstractChannelBuffer.h"

namespace cetty { namespace buffer {

/**
 * A dynamic capacity buffer which increases its capacity as needed.  It is
 * recommended to use {@link ChannelBuffers#dynamicBuffer(int)} instead of
 * calling the constructor explicitly.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class DynamicChannelBuffer : public AbstractChannelBuffer {
public:
    DynamicChannelBuffer(int estimatedLength);

    DynamicChannelBuffer(ByteOrder endianness, int estimatedLength);

    DynamicChannelBuffer(ByteOrder endianness, int estimatedLength, ChannelBufferFactory& factory);

    virtual ~DynamicChannelBuffer() {}

    virtual void ensureWritableBytes(int minWritableBytes);

    virtual ChannelBufferFactory& factory() const {
        return this->bufferFactory;
    }

    virtual ByteOrder order() const {
        return endianness;
    }

    virtual int capacity() const {
        return buffer->capacity();
    }

    virtual int  readableBytes() const {
        return AbstractChannelBuffer::readableBytes();
    }
    virtual void readableBytes(Array& array) {}
    virtual int  writableBytes() const {
        return AbstractChannelBuffer::writableBytes();
    }
    virtual void writableBytes(Array& array) {}

    virtual bool hasArray() const {
        return buffer->hasArray();
    }

    virtual const Array& array() {
        return buffer->array();
    }

    virtual ConstArray array() const {
        return buffer->array();
    }

    virtual int arrayOffset() const {
        return buffer->arrayOffset();
    }

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

    virtual void getBytes(int index, const Array& dst, int dstIndex, int length) const {
        buffer->getBytes(index, dst, dstIndex, length);
    }

    virtual void getBytes(int index, ChannelBuffer& dst, int dstIndex, int length) const {
        buffer->getBytes(index, dst, dstIndex, length);
    }

    virtual void getBytes(int index, OutputStream& out, int length) const {
        buffer->getBytes(index, out, length);
    }

    virtual void setByte(int index, int value) {
        buffer->setByte(index, value);
    }

    virtual void setShort(int index, int value) {
        buffer->setShort(index, value);
    }

    virtual void setMedium(int index, int value) {
        buffer->setMedium(index, value);
    }

    virtual void setInt(int index, int value) {
        buffer->setInt(index, value);
    }

    virtual void setLong(int index, boost::int64_t value) {
        buffer->setLong(index, value);
    }

    virtual void setBytes(int index, const ConstArray& src, int srcIndex, int length) {
        buffer->setBytes(index, src, srcIndex, length);
    }

    virtual void setBytes(int index, const ChannelBuffer& src, int srcIndex, int length) {
        buffer->setBytes(index, src, srcIndex, length);
    }

    virtual int setBytes(int index, InputStream& in, int length) {
        return buffer->setBytes(index, in, length);
    }

    virtual void writeByte(int value) {
        ensureWritableBytes(1);
        AbstractChannelBuffer::writeByte(value);
    }

    virtual void writeShort(int value) {
        ensureWritableBytes(2);
        AbstractChannelBuffer::writeShort(value);
    }

    virtual void writeMedium(int value) {
        ensureWritableBytes(3);
        AbstractChannelBuffer::writeMedium(value);
    }

    virtual void writeInt(int value) {
        ensureWritableBytes(4);
        AbstractChannelBuffer::writeInt(value);
    }

    virtual void writeLong(boost::int64_t value) {
        ensureWritableBytes(8);
        AbstractChannelBuffer::writeLong(value);
    }

    virtual void writeBytes(const ConstArray& src, int srcIndex, int length) {
        ensureWritableBytes(length);
        AbstractChannelBuffer::writeBytes(src, srcIndex, length);
    }

    virtual void writeBytes(const ChannelBuffer& src, int srcIndex, int length) {
        ensureWritableBytes(length);
        AbstractChannelBuffer::writeBytes(src, srcIndex, length);
    }

    virtual int writeBytes(InputStream& in, int length) {
        ensureWritableBytes(length);
        return AbstractChannelBuffer::writeBytes(in, length);
    }

    virtual void writeZero(int length) {
        ensureWritableBytes(length);
        AbstractChannelBuffer::writeZero(length);
    }

    virtual ChannelBufferPtr duplicate();
    virtual ChannelBufferPtr copy(int index, int length) const;
    virtual ChannelBufferPtr slice(int index, int length);
    virtual void slice(Array& array) {}

    virtual void readSlice(Array& array){}
    virtual void readSlice(GatheringBuffer& gathering) {}

private:
    ByteOrder endianness;
    ChannelBufferFactory& bufferFactory;
    ChannelBufferPtr buffer;
};

}}

#endif //#if !defined(CETTY_BUFFER_DYNAMICCHANNELBUFFER_H)
