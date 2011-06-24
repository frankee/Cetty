#if !defined(CETTY_BUFFER_TRUNCATEDCHANNELBUFFER_H)
#define CETTY_BUFFER_TRUNCATEDCHANNELBUFFER_H

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
 * A derived buffer which hides its parent's tail data beyond a certain index.
 * It is recommended to use {@link ChannelBuffer#slice()} and
 * {@link ChannelBuffer#slice(int, int)} instead of calling the constructor
 * explicitly.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class TruncatedChannelBuffer : public AbstractChannelBuffer, public WrappedChannelBuffer {
public:
    TruncatedChannelBuffer(const ChannelBufferPtr& buffer, int length);

    virtual ~TruncatedChannelBuffer() {}

    virtual ChannelBufferPtr& unwrap() {
        return this->buffer;
    }

    virtual ChannelBufferFactory& factory() const {
        return this->buffer->factory();
    }

    virtual ByteOrder order() const {
        return buffer->order();
    }

    virtual int capacity() const {
        return this->length;
    }

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

    virtual void readableBytes(Array& array);
    virtual void writableBytes(Array& array);

    virtual boost::int8_t  getByte(int index) const;
    virtual boost::int16_t getShort(int index) const;
    virtual boost::int32_t getUnsignedMedium(int index) const;
    virtual boost::int32_t getInt(int index) const;
    virtual boost::int64_t getLong(int index) const;

    virtual ChannelBufferPtr duplicate();
    virtual ChannelBufferPtr copy(int index, int length) const;
    virtual ChannelBufferPtr slice(int index, int length);
    virtual void slice(Array& array);

    virtual void getBytes(int index, ChannelBuffer& dst, int dstIndex, int length) const;
    virtual void getBytes(int index, const Array& dst, int dstIndex, int length) const;
    virtual void getBytes(int index, OutputStream& out, int length) const;

    virtual void setByte(int index, int value);
    virtual void setShort(int index, int value);
    virtual void setMedium(int index, int value);
    virtual void setInt(int index, int value);
    virtual void setLong(int index, boost::int64_t value);

    virtual void setBytes(int index, const ConstArray& src, int srcIndex, int length);
    virtual void setBytes(int index, const ChannelBuffer& src, int srcIndex, int length);
    virtual int  setBytes(int index, InputStream& in, int length);
    
    virtual void readSlice(Array& array){};
    virtual void readSlice(GatheringBuffer& gathering) {}

private:
    void checkIndex(int index) const;
    void checkIndex(int index, int length) const;

private:
    ChannelBufferPtr buffer;
    int length;
};

}}

#endif //#if !defined(CETTY_BUFFER_TRUNCATEDCHANNELBUFFER_H)
