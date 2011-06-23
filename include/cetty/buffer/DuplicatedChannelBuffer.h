#if !defined(CETTY_BUFFER_DUPLICATEDCHANNELBUFFER_H)
#define CETTY_BUFFER_DUPLICATEDCHANNELBUFFER_H

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
#include "cetty/buffer/WrappedChannelBuffer.h"

namespace cetty { namespace buffer {

/**
 * A derived buffer which simply forwards all data access requests to its
 * parent.  It is recommended to use {@link ChannelBuffer#duplicate()} instead
 * of calling the constructor explicitly.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2206 $, $Date: 2010-03-03 14:35:01 +0900 (Wed, 03 Mar 2010) $
 *
 */

class DuplicatedChannelBuffer : public AbstractChannelBuffer, public WrappedChannelBuffer {
public:
    DuplicatedChannelBuffer(const ChannelBufferPtr& buffer) : buffer(buffer) {
        setIndex(buffer->readerIndex(), buffer->writerIndex());
    }

    virtual ~DuplicatedChannelBuffer() {}

    virtual ChannelBufferPtr& unwrap() {
        return buffer;
    }

    virtual ChannelBufferFactory& factory() const {
        return buffer->factory();
    }

    virtual ByteOrder order() const {
        return buffer->order();
    }

    virtual int capacity() const {
        return buffer->capacity();
    }

    virtual void readableBytes(Array& array) {}
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

    virtual boost::int8_t getByte(int index) {
        return buffer->getByte(index);
    }

    virtual ChannelBufferPtr duplicate() {
        return ChannelBufferPtr(new DuplicatedChannelBuffer(*this));
    }

    virtual ChannelBufferPtr copy(int index, int length) const {
        return buffer->copy(index, length);
    }

    virtual ChannelBufferPtr slice(int index, int length) {
        return buffer->slice(index, length);
    }

    virtual void slice(Array& array) {}

    virtual void getBytes(int index, ChannelBuffer& dst, int dstIndex, int length) const {
        buffer->getBytes(index, dst, dstIndex, length);
    }

    virtual void getBytes(int index, const Array& dst, int dstIndex, int length) const {
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

    virtual void readSlice(Array& array){}
    virtual void readSlice(GatheringBuffer& gathering) {}

private:
    DuplicatedChannelBuffer(DuplicatedChannelBuffer& buff)
        : buffer(buff.buffer) {
        setIndex(buff.buffer->readerIndex(), buff.buffer->writerIndex());
    }

private:
    ChannelBufferPtr buffer;
};

}}

#endif //#if !defined(CETTY_BUFFER_DUPLICATEDCHANNELBUFFER_H)
