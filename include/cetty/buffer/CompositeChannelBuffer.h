#if !defined(CETTY_BUFFER_COMPOSITECHANNELBUFFER_H)
#define CETTY_BUFFER_COMPOSITECHANNELBUFFER_H

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

#include <vector>
#include "cetty/buffer/AbstractChannelBuffer.h"

namespace cetty { namespace buffer {

class CompositeChannelBuffer;

/**
 * A virtual buffer which shows multiple buffers as a single merged buffer.  It
 * is recommended to use {@link ChannelBuffers#wrappedBuffer(ChannelBuffer...)}
 * instead of calling the constructor explicitly.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author Frederic Bregier (fredbregier@free.fr)
 *
 * @version $Rev: 2206 $, $Date: 2010-03-03 14:35:01 +0900 (Wed, 03 Mar 2010) $
 *
 */

typedef boost::intrusive_ptr<CompositeChannelBuffer> CompositeChannelBufferPtr;

class CompositeChannelBuffer : public AbstractChannelBuffer {
public:
	CompositeChannelBuffer(ByteOrder endianness,
                           const std::vector<ChannelBufferPtr>& buffers)
        : byteOrder(endianness) {
        setComponents(buffers);
    }

    virtual ~CompositeChannelBuffer() {}

    std::vector<ChannelBufferPtr> decompose() {
        return decompose(readerIndex(), readableBytes());
    }

    /**
     * Same with {@link #slice(int, int)} except that this method returns a list.
     */
    std::vector<ChannelBufferPtr> decompose(int index, int length);

    virtual ChannelBufferFactory& factory() const;

    virtual ByteOrder order() const;

    virtual int readableBytes() const {
        return AbstractChannelBuffer::readableBytes();
    }
    virtual void readableBytes(Array& array) {}

    virtual int writableBytes() const {
        return AbstractChannelBuffer::writableBytes();
    }
    virtual void writableBytes(Array& array) {}

    virtual bool hasArray() const;
    virtual const Array& array();
    virtual ConstArray array() const;
    virtual int arrayOffset() const;

    virtual int capacity() const;

    virtual boost::int8_t getByte(int index) const;
    virtual boost::int16_t getShort(int index) const;
    virtual boost::int32_t getUnsignedMedium(int index) const;
    virtual boost::int32_t getInt(int index) const;
    virtual boost::int64_t getLong(int index) const;
    virtual void getBytes(int index, const Array& dst, int dstIndex, int length) const;

    virtual void getBytes(int index, ChannelBuffer& dst, int dstIndex, int length) const;
    virtual void getBytes(int index, OutputStream& out, int length) const;

    virtual void setByte(int index, int value);
    virtual void setShort(int index, int value);
    virtual void setMedium(int index, int value);
    virtual void setInt(int index, int value);
    virtual void setLong(int index, boost::int64_t value);
    virtual void setBytes(int index, const ConstArray& src, int srcIndex, int length);
    virtual void setBytes(int index, const ChannelBuffer& src, int srcIndex, int length);
    virtual int setBytes(int index, InputStream& in, int length);

    virtual ChannelBufferPtr duplicate();
    virtual ChannelBufferPtr copy(int index, int length) const;
    virtual ChannelBufferPtr slice(int index, int length);
    virtual void slice(Array& array) {}

    virtual void readSlice(Array& array){}
    virtual void readSlice(GatheringBuffer& gathering) {}

    virtual void discardReadBytes();

    virtual std::string toString();

private:
    /**
     * Setup this ChannelBuffer from the list
     */
    void setComponents(const std::vector<ChannelBufferPtr>& newComponents);

    void copyTo(int index, int length, int componentId, ChannelBuffer& dst) const;

    int getComponentId(int index) const;
    int getComponentId(int index);

    CompositeChannelBuffer(CompositeChannelBuffer& buffer)
        : byteOrder(buffer.byteOrder),
          components(buffer.components),
          indices(buffer.indices),
          lastAccessedComponentId(0) {
        
        setIndex(buffer.readerIndex(), buffer.writerIndex());
    }

private:
    ByteOrder byteOrder;
    std::vector<ChannelBufferPtr> components;
    std::vector<int> indices;
    int lastAccessedComponentId;
};

}}

#endif //#if !defined(CETTY_BUFFER_COMPOSITECHANNELBUFFER_H)
