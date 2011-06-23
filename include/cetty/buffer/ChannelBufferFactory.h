#if !defined(CETTY_BUFFER_CHANNELBUFFERFACTORY_H)
#define CETTY_BUFFER_CHANNELBUFFERFACTORY_H

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

#include "cetty/buffer/ChannelBuffer.h"
#include "cetty/buffer/ByteOrder.h"
#include "cetty/buffer/Array.h"

namespace cetty { namespace buffer {

/**
 * A factory that creates or pools {@link ChannelBuffer}s.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */

class ChannelBufferFactory {
public:
    /**
     * Returns a {@link ChannelBuffer} with the specified <tt>capacity</tt>.
     * This method is identical to <tt>getBuffer(getDefaultOrder(), capacity)</tt>.
     *
     * @param capacity the capacity of the returned {@link ChannelBuffer}
     * @return a {@link ChannelBuffer} with the specified <tt>capacity</tt>,
     *         whose <tt>readerIndex</tt> and <tt>writerIndex</tt> are <tt>0</tt>
     */
    virtual ChannelBufferPtr getBuffer(int capacity) = 0;

    /**
     * Returns a {@link ChannelBuffer} with the specified <tt>endianness</tt>
     * and <tt>capacity</tt>.
     *
     * @param endianness the endianness of the returned {@link ChannelBuffer}
     * @param capacity   the capacity of the returned {@link ChannelBuffer}
     * @return a {@link ChannelBuffer} with the specified <tt>endianness</tt> and
     *         <tt>capacity</tt>, whose <tt>readerIndex</tt> and <tt>writerIndex</tt>
     *         are <tt>0</tt>
     */
    virtual ChannelBufferPtr getBuffer(ByteOrder endianness, int capacity) = 0;

    /**
     * Returns a {@link ChannelBuffer} whose content is equal to the sub-region
     * of the specified <tt>array</tt>.  Depending on the factory implementation,
     * the returned buffer could wrap the <tt>array</tt> or create a new copy of
     * the <tt>array</tt>.
     * This method is identical to <tt>getBuffer(getDefaultOrder(), array, offset, length)</tt>.
     *
     * @param array the byte array
     * @param offset the offset of the byte array
     * @param length the length of the byte array
     *
     * @return a {@link ChannelBuffer} with the specified content,
     *         whose <tt>readerIndex</tt> and <tt>writerIndex</tt>
     *         are <tt>0</tt> and <tt>(length - offset)</tt> respectively
     */
    virtual ChannelBufferPtr getBuffer(const Array& array, int offset, int length) = 0;

    /**
     * Returns a {@link ChannelBuffer} whose content is equal to the sub-region
     * of the specified <tt>array</tt>.  Depending on the factory implementation,
     * the returned buffer could wrap the <tt>array</tt> or create a new copy of
     * the <tt>array</tt>.
     *
     * @param endianness the endianness of the returned {@link ChannelBuffer}
     * @param array the byte array
     * @param offset the offset of the byte array
     * @param length the length of the byte array
     *
     * @return a {@link ChannelBuffer} with the specified content,
     *         whose <tt>readerIndex</tt> and <tt>writerIndex</tt>
     *         are <tt>0</tt> and <tt>(length - offset)</tt> respectively
     */
    virtual ChannelBufferPtr getBuffer(ByteOrder endianness, const Array& array, int offset, int length) = 0;

    /**
     * Returns the default endianness of the {@link ChannelBuffer} which is
     * returned by {@link #getBuffer(int)}.
     *
     * @return the default endianness of the {@link ChannelBuffer} which is
     *         returned by {@link #getBuffer(int)}
     */
    virtual ByteOrder getDefaultOrder() const = 0;

protected:
    virtual ~ChannelBufferFactory() {}
};

}}

#endif //#if !defined(CETTY_BUFFER_CHANNELBUFFERFACTORY_H)
