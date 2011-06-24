#if !defined(CETTY_BUFFER_HEAPCHANNELBUFFER_H)
#define CETTY_BUFFER_HEAPCHANNELBUFFER_H

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
 * A skeletal implementation for Java heap buffers.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class HeapChannelBuffer : public AbstractChannelBuffer {
public:
    /**
     * Creates a new heap buffer with a newly allocated byte array.
     *
     * @param length the length of the new byte array
     */
    explicit HeapChannelBuffer(int length);

    /**
     * Creates a new heap buffer with an existing byte array.
     *
     * @param buf the char pointer
     * @param length the length of the buf.
     */
    explicit HeapChannelBuffer(char* buf, int length);

    /**
     * Creates a new heap buffer with an existing byte array.
     *
     * @param array the byte array to wrap
     */
    explicit HeapChannelBuffer(const Array& array);


    explicit HeapChannelBuffer(const Array& array, bool maintainedBuf);

    virtual ~HeapChannelBuffer();

    virtual int  capacity() const;

    virtual bool hasArray() const;

    virtual const Array& array() {
        return this->arry;
    }

    virtual ConstArray array() const {
        return this->arry;
    }

    virtual int arrayOffset() const {
        return 0;
    }

    virtual void readableBytes(Array& array);
    virtual void writableBytes(Array& array);

    // need not to call the virtual function of capacity()
    virtual void offsetWriterIndex(int offset);

    virtual boost::int8_t getByte(int index) const;

    virtual void getBytes(int index, ChannelBuffer& dst, int dstIndex, int length) const;
    virtual void getBytes(int index, const Array& dst, int dstIndex, int length) const;
    virtual void getBytes(int index, OutputStream& out, int length) const;

    virtual void setByte(int index, int value);
    virtual void setBytes(int index, const ChannelBuffer& src, int srcIndex, int length);
    virtual void setBytes(int index, const ConstArray& src, int srcIndex, int length);
    virtual int  setBytes(int index, InputStream& in, int length);

    virtual void readSlice(Array& array);
    virtual void readSlice(GatheringBuffer& gatheringBuffer);

    virtual ChannelBufferPtr slice(int index, int length);
    virtual void slice(Array& array);

protected:
    /**
     * Creates a new heap buffer with an existing byte array.
     *
     * @param array        the byte array to wrap
     * @param readerIndex  the initial reader index of this buffer
     * @param writerIndex  the initial writer index of this buffer
     */
    HeapChannelBuffer(const Array& array, int readerIndex, int writerIndex)
        :  maintainArrayBuffer(false), arry(array) {
        setIndex(readerIndex, writerIndex);
    }

protected:
    /**
     *	Indicated whether to maintain the life cycle of
     *  the underlying heap byte or not.
     */
    bool maintainArrayBuffer;

    /**
     * The underlying heap byte array that this buffer is wrapping.
     */
     Array arry;
};

}}


#endif //#if !defined(CETTY_BUFFER_HEAPCHANNELBUFFER_H)
