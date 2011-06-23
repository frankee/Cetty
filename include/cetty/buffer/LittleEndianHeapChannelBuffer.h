#if !defined(CETTY_BUFFER_LITTLEENDIANHEAPCHANNELBUFFER_H)
#define CETTY_BUFFER_LITTLEENDIANHEAPCHANNELBUFFER_H

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

#include "cetty/buffer/HeapChannelBuffer.h"

namespace cetty { namespace buffer {


/**
 * A little-endian Java heap buffer.  It is recommended to use {@link ChannelBuffers#buffer(ByteOrder, int)}
 * and {@link ChannelBuffers#wrappedBuffer(ByteOrder, byte[])} instead of
 * calling the constructor explicitly.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2206 $, $Date: 2010-03-03 14:35:01 +0900 (Wed, 03 Mar 2010) $
 */

class LittleEndianHeapChannelBuffer : public HeapChannelBuffer {
public:
    /**
     * Creates a new little-endian heap buffer with a newly allocated byte array.
     *
     * @param length the length of the new byte array
     */
    explicit LittleEndianHeapChannelBuffer(int length) : HeapChannelBuffer(length) {
    }

    /**
     * Creates a new little-endian heap buffer with an existing byte array.
     *
     * @param array the byte array to wrap
     */
    explicit LittleEndianHeapChannelBuffer(const Array& array)
        : HeapChannelBuffer(array) {}

    explicit LittleEndianHeapChannelBuffer(const Array& array, bool maintainedBuf)
        : HeapChannelBuffer(array, maintainedBuf) {}

    virtual ~LittleEndianHeapChannelBuffer() {}

    virtual ChannelBufferFactory& factory() const;
    virtual ByteOrder order() const;

    virtual boost::int16_t getShort(int index) const;
    virtual boost::int32_t getUnsignedMedium(int index) const;
    virtual boost::int32_t getInt(int index) const;
    virtual boost::int64_t getLong(int index) const;

    virtual void setShort(int index, int value);
    virtual void setMedium(int index, int   value);
    virtual void setInt(int index, int   value);
    virtual void setLong(int index, boost::int64_t  value);

    virtual ChannelBufferPtr duplicate();
    virtual ChannelBufferPtr copy(int index, int length) const;

private:
    LittleEndianHeapChannelBuffer(const Array& array, int readerIndex, int writerIndex)
        : HeapChannelBuffer(array, readerIndex, writerIndex) {
    }
};

}}
#endif //#if !defined(CETTY_BUFFER_LITTLEENDIANHEAPCHANNELBUFFER_H)
