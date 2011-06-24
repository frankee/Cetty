#if !defined(CETTY_BUFFER_CHANNELBUFFERS_H)
#define CETTY_BUFFER_CHANNELBUFFERS_H

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
#include "cetty/buffer/ChannelBuffer.h"

namespace cetty { namespace util {
class Charset;
}}

namespace cetty { namespace buffer {

using namespace cetty::util;

/**
 * Creates a new {@link ChannelBuffer} by allocating new space or by wrapping
 * or copying existing byte arrays, byte buffers and a string.
 *
 * <h3>Use static import</h3>
 * This classes is intended to be used with Java 5 static import statement:
 *
 * <pre>
 * #include "cetty/buffer/ChannelBuffers.h"
 *
 * {@link ChannelBuffer} *heapBuffer    = ChannelBuffers::buffer(128);
 * {@link ChannelBuffer} *directBuffer  = ChannelBuffers::directBuffer(256);
 * {@link ChannelBuffer} *dynamicBuffer = ChannelBuffers::dynamicBuffer(512);
 * {@link ChannelBuffer} *wrappedBuffer = ChannelBuffers::wrappedBuffer(new byte[128], new byte[256]);
 * {@link ChannelBuffer} *copiedBuffe r = ChannelBuffers::copiedBuffer({@link ByteBuffer}.allocate(128));
 * </pre>
 *
 * <h3>Allocating a new buffer</h3>
 *
 * Three buffer types are provided out of the box.
 *
 * <ul>
 * <li>{@link #buffer(int)} allocates a new fixed-capacity heap buffer.</li>
 * <li>{@link #directBuffer(int)} allocates a new fixed-capacity direct buffer.</li>
 * <li>{@link #dynamicBuffer(int)} allocates a new dynamic-capacity heap
 *     buffer, whose capacity increases automatically as needed by a write
 *     operation.</li>
 * </ul>
 *
 * <h3>Creating a wrapped buffer</h3>
 *
 * Wrapped buffer is a buffer which is a view of one or more existing
 * byte arrays and byte buffers.  Any changes in the content of the original
 * array or buffer will be visible in the wrapped buffer.  Various wrapper
 * methods are provided and their name is all <tt>wrappedBuffer()</tt>.
 * You might want to take a look at the methods that accept varargs closely if
 * you want to create a buffer which is composed of more than one array to
 * reduce the number of memory copy.
 *
 * <h3>Creating a copied buffer</h3>
 *
 * Copied buffer is a deep copy of one or more existing byte arrays, byte
 * buffers or a string.  Unlike a wrapped buffer, there's no shared data
 * between the original data and the copied buffer.  Various copy methods are
 * provided and their name is all <tt>copiedBuffer()</tt>.  It is also convenient
 * to use this operation to merge multiple buffers into one buffer.
 *
 * <h3>Miscellaneous utility methods</h3>
 *
 * This class also provides various utility methods to help implementation
 * of a new buffer type, generation of hex dump and swapping an integer's
 * byte order.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 * @apiviz.has org.jboss.netty.buffer.ChannelBuffer oneway - - creates
 */

class ChannelBuffers {
public:
    /**
     * A buffer whose capacity is <tt>0</tt>.
     */
    static ChannelBufferPtr EMPTY_BUFFER;

    /**
     * Creates a new big-endian Java heap buffer with the specified
     * <tt>capacity</tt>.  The new buffer's <tt>readerIndex</tt> and
     * <tt>writerIndex</tt> are <tt>0</tt>.
     */
    static ChannelBufferPtr buffer(int capacity) {
        return ChannelBuffers::buffer(ByteOrder::BYTE_ORDER_BIG, capacity);
    }

    /**
     * Creates a new Java heap buffer with the specified <tt>endianness</tt>
     * and <tt>capacity</tt>.  The new buffer's <tt>readerIndex</tt> and
     * <tt>writerIndex</tt> are <tt>0</tt>.
     */
    static ChannelBufferPtr buffer(ByteOrder endianness, int capacity);

    /**
     * Creates a new big-endian dynamic buffer whose estimated data length is
     * <tt>256</tt> bytes.  The new buffer's <tt>readerIndex</tt> and
     * <tt>writerIndex</tt> are <tt>0</tt>.
     */
    static ChannelBufferPtr dynamicBuffer() {
        return ChannelBuffers::dynamicBuffer(ByteOrder::BYTE_ORDER_BIG, 256);
    }

    static ChannelBufferPtr dynamicBuffer(ChannelBufferFactory& factory);

    /**
     * Creates a new big-endian dynamic buffer with the specified estimated
     * data length.  More accurate estimation yields less unexpected
     * reallocation overhead.  The new buffer's <tt>readerIndex</tt> and
     * <tt>writerIndex</tt> are <tt>0</tt>.
     */
    static ChannelBufferPtr dynamicBuffer(int estimatedLength) {
        return ChannelBuffers::dynamicBuffer(ByteOrder::BYTE_ORDER_BIG, estimatedLength);
    }

    /**
     * Creates a new dynamic buffer with the specified endianness and
     * the specified estimated data length.  More accurate estimation yields
     * less unexpected reallocation overhead.  The new buffer's
     * <tt>readerIndex</tt> and <tt>writerIndex</tt> are <tt>0</tt>.
     */
    static ChannelBufferPtr dynamicBuffer(ByteOrder endianness, int estimatedLength);

    /**
     * Creates a new big-endian dynamic buffer with the specified estimated
     * data length using the specified factory.  More accurate estimation yields
     * less unexpected reallocation overhead.  The new buffer's <tt>readerIndex</tt>
     * and <tt>writerIndex</tt> are <tt>0</tt>.
     */
    static ChannelBufferPtr dynamicBuffer(int estimatedLength, ChannelBufferFactory& factory);

    /**
     * Creates a new dynamic buffer with the specified endianness and
     * the specified estimated data length using the specified factory.
     * More accurate estimation yields less unexpected reallocation overhead.
     * The new buffer's <tt>readerIndex</tt> and <tt>writerIndex</tt> are <tt>0</tt>.
     */
    static ChannelBufferPtr dynamicBuffer(ByteOrder endianness, int estimatedLength, ChannelBufferFactory& factory);
    
    /**
     * Creates a new big-endian buffer which wraps the specified <tt>string</tt>.
     * A modification on the specified array's content will be visible to the
     * returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(std::string& str) {
        return ChannelBuffers::wrappedBuffer(Array((char*)str.data(), (int)str.size()));
    }

    /**
     * Creates a new big-endian buffer which wraps the specified <tt>array</tt>.
     * A modification on the specified array's content will be visible to the
     * returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(const Array& array) {
        return ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_BIG, array);
    }

    /**
     * Creates a new buffer which wraps the specified <tt>array</tt> with the
     * specified <tt>endianness</tt>.  A modification on the specified array's
     * content will be visible to the returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(ByteOrder endianness, const Array& array);

    /**
     * Creates a new big-endian buffer which wraps the sub-region of the
     * specified <tt>array</tt>.  A modification on the specified array's
     * content will be visible to the returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(const Array& array, int offset, int length) {
        return ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_BIG, array, offset, length);
    }

    /**
     * Creates a new buffer which wraps the sub-region of the specified
     * <tt>array</tt> with the specified <tt>endianness</tt>.  A modification on
     * the specified array's content will be visible to the returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(ByteOrder endianness, const Array& array, int offset, int length);

    /**
     * Creates a new big-endian composite buffer which wraps the specified
     * arrays without copying them.  A modification on the specified arrays'
     * content will be visible to the returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(const Array& array0, const Array& array1) {
        return wrappedBuffer(ByteOrder::BYTE_ORDER_BIG, array0, array1);
    }

    /**
     * Creates a new big-endian composite buffer which wraps the specified
     * arrays without copying them.  A modification on the specified arrays'
     * content will be visible to the returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(const Array& array0, const Array& array1, const Array& array2) {
        return wrappedBuffer(ByteOrder::BYTE_ORDER_BIG, array0, array1, array2);
    }

    /**
     * Creates a new big-endian composite buffer which wraps the specified
     * arrays without copying them.  A modification on the specified arrays'
     * content will be visible to the returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(const Array& array0, const Array& array1, const Array& array2, const Array& array3) {
        return wrappedBuffer(ByteOrder::BYTE_ORDER_BIG, array0, array1, array2, array3);
    }

    /**
     * Creates a new composite buffer which wraps the specified arrays without
     * copying them.  A modification on the specified arrays' content will be
     * visible to the returned buffer.
     *
     * @param endianness the endianness of the new buffer
     */
    static ChannelBufferPtr wrappedBuffer(ByteOrder endianness, const Array& array0, const Array& array1);

    /**
     * Creates a new composite buffer which wraps the specified arrays without
     * copying them.  A modification on the specified arrays' content will be
     * visible to the returned buffer.
     *
     * @param endianness the endianness of the new buffer
     */
    static ChannelBufferPtr wrappedBuffer(ByteOrder endianness, const Array& array0, const Array& array1, const Array& array2);
    
    /**
     * Creates a new composite buffer which wraps the specified arrays without
     * copying them.  A modification on the specified arrays' content will be
     * visible to the returned buffer.
     *
     * @param endianness the endianness of the new buffer
     */
    static ChannelBufferPtr wrappedBuffer(ByteOrder endianness, const Array& array0, const Array& array1, const Array& array2, const Array& array3);

    /**
     * Creates a new composite buffer which wraps the specified arrays without
     * copying them.  A modification on the specified arrays' content will be
     * visible to the returned buffer.
     *
     * @param endianness the endianness of the new buffer
     */
    static ChannelBufferPtr wrappedBuffer(std::vector<Array>& arrays) {
        return wrappedBuffer(ByteOrder::BYTE_ORDER_BIG, arrays);
    }

    /**
     * Creates a new composite buffer which wraps the specified arrays without
     * copying them.  A modification on the specified arrays' content will be
     * visible to the returned buffer.
     *
     * @param endianness the endianness of the new buffer
     */
    static ChannelBufferPtr wrappedBuffer(ByteOrder endianness, std::vector<Array>& arrays);

    /**
     * Creates a new buffer which wraps the specified buffer's readable bytes.
     * A modification on the specified buffer's content will be visible to the
     * returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(const ChannelBufferPtr& buffer);

    /**
     * Creates a new composite buffer which wraps the readable bytes of the
     * specified buffers without copying them.  A modification on the content
     * of the specified buffers will be visible to the returned buffer.
     *
     * @throws InvalidArgumentException
     *         if the specified buffers' endianness are different from each
     *         other
     */
    static ChannelBufferPtr wrappedBuffer(const ChannelBufferPtr& buffer0,
                                          const ChannelBufferPtr& buffer1);
    
    static ChannelBufferPtr wrappedBuffer(const ChannelBufferPtr& buffer0,
                                          const ChannelBufferPtr& buffer1,
                                          const ChannelBufferPtr& buffer2);
    
    static ChannelBufferPtr wrappedBuffer(const ChannelBufferPtr& buffer0,
                                          const ChannelBufferPtr& buffer1,
                                          const ChannelBufferPtr& buffer2,
                                          const ChannelBufferPtr& buffer3);

    static ChannelBufferPtr wrappedBuffer(const std::vector<ChannelBufferPtr>& buffers);

    /**
     * Creates a new buffer with the specified <tt>endianness</tt> whose
     * content is a copy of the specified <tt>array</tt>.  The new buffer's
     * <tt>readerIndex</tt> and <tt>writerIndex</tt> are <tt>0</tt> and
     * <tt>array.length()</tt> respectively.
     */
    static ChannelBufferPtr copiedBuffer(ByteOrder endianness, const ConstArray& array);

    /**
     * Creates a new big-endian buffer whose content is a copy of the
     * specified <tt>array</tt>.  The new buffer's <tt>readerIndex</tt> and
     * <tt>writerIndex</tt> are <tt>0</tt> and <tt>array.length()</tt> respectively.
     */
    static ChannelBufferPtr copiedBuffer(const ConstArray& array) {
        return copiedBuffer(ByteOrder::BYTE_ORDER_BIG, array);
    }

    /**
     * Creates a new big-endian buffer whose content is a copy of the
     * specified <tt>array</tt>'s sub-region.  The new buffer's
     * <tt>readerIndex</tt> and <tt>writerIndex</tt> are <tt>0</tt> and
     * the specified <tt>length</tt> respectively.
     */
    static ChannelBufferPtr copiedBuffer(const ConstArray& array, int offset, int length) {
        return copiedBuffer(ByteOrder::BYTE_ORDER_BIG, array, offset, length);
    }

    /**
     * Creates a new buffer with the specified <tt>endianness</tt> whose
     * content is a copy of the specified <tt>array</tt>'s sub-region.  The new
     * buffer's <tt>readerIndex</tt> and <tt>writerIndex</tt> are <tt>0</tt> and
     * the specified <tt>length</tt> respectively.
     */
    static ChannelBufferPtr copiedBuffer(ByteOrder endianness, const ConstArray& array, int offset, int length);

    /**
     * Creates a new big-endian buffer whose content is a merged copy of
     * the specified <tt>arrays</tt>.  The new buffer's <tt>readerIndex</tt>
     * and <tt>writerIndex</tt> are <tt>0</tt> and the sum of all arrays'
     * <tt>length</tt> respectively.
     */
    static ChannelBufferPtr copiedBuffer(const Array& array0, const Array& array1) {
        return copiedBuffer(ByteOrder::BYTE_ORDER_BIG, array0, array1);
    }
    static ChannelBufferPtr copiedBuffer(const Array& array0, const Array& array1, const Array& array2) {
        return copiedBuffer(ByteOrder::BYTE_ORDER_BIG, array0, array1, array2);
    }
    static ChannelBufferPtr copiedBuffer(const Array& array0, const Array& array1, const Array& array2, const Array& array3) {
        return copiedBuffer(ByteOrder::BYTE_ORDER_BIG, array0, array1, array2, array3);
    }

    static ChannelBufferPtr copiedBuffer(const std::vector<Array>& arrays) {
        return copiedBuffer(ByteOrder::BYTE_ORDER_BIG, arrays);
    }

    /**
     * Creates a new buffer with the specified <tt>endianness</tt> whose
     * content is a merged copy of the specified <tt>arrays</tt>.  The new
     * buffer's <tt>readerIndex</tt> and <tt>writerIndex</tt> are <tt>0</tt>
     * and the sum of all arrays' <tt>length</tt> respectively.
     */
    static ChannelBufferPtr copiedBuffer(ByteOrder endianness, const Array& array0, const Array& array1);
    static ChannelBufferPtr copiedBuffer(ByteOrder endianness, const Array& array0, const Array& array1, const Array& array2);
    static ChannelBufferPtr copiedBuffer(ByteOrder endianness, const Array& array0, const Array& array1, const Array& array2, const Array& array3);
    static ChannelBufferPtr copiedBuffer(ByteOrder endianness, const std::vector<Array>& arrays);
    
    /**
     * Creates a new buffer whose content is a copy of the specified
     * <tt>buffer</tt>'s readable bytes.  The new buffer's <tt>readerIndex</tt>
     * and <tt>writerIndex</tt> are <tt>0</tt> and <tt>buffer.readableBytes</tt>
     * respectively.
     */
    static ChannelBufferPtr copiedBuffer(const ChannelBufferPtr& buffer);

    /**
     * Creates a new buffer whose content is a merged copy of the specified
     * <tt>buffers</tt>' readable bytes.  The new buffer's <tt>readerIndex</tt>
     * and <tt>writerIndex</tt> are <tt>0</tt> and the sum of all buffers'
     * <tt>readableBytes</tt> respectively.
     *
     * @throws InvalidArgumentException
     *         if the specified buffers' endianness are different from each
     *         other
     */
    static ChannelBufferPtr copiedBuffer(const ChannelBufferPtr& buffer0,
                                         const ChannelBufferPtr& buffer1);

    static ChannelBufferPtr copiedBuffer(const ChannelBufferPtr& buffer0,
                                         const ChannelBufferPtr& buffer1,
                                         const ChannelBufferPtr& buffer2);

    static ChannelBufferPtr copiedBuffer(const ChannelBufferPtr& buffer0,
                                         const ChannelBufferPtr& buffer1,
                                         const ChannelBufferPtr& buffer2,
                                         const ChannelBufferPtr& buffer3);

    static ChannelBufferPtr copiedBuffer(const std::vector<ChannelBufferPtr>& buffers);


    static ChannelBufferPtr copiedBuffer(const std::string& string) {
        return copiedBuffer(ByteOrder::BYTE_ORDER_BIG, ConstArray::fromString(string));
    }

    /**
     * Creates a new buffer with the specified <tt>endianness</tt> whose
     * content is the specified <tt>string</tt> encoded in the specified
     * <tt>charset</tt>.  The new buffer's <tt>readerIndex</tt> and
     * <tt>writerIndex</tt> are <tt>0</tt> and the length of the encoded string
     * respectively.
     */
    static ChannelBufferPtr copiedBuffer(ByteOrder endianness, const std::string& string) {
        return copiedBuffer(endianness, ConstArray::fromString(string));
    }

    /**
     * Creates a new big-endian buffer whose content is a subregion of
     * the specified <tt>array</tt> encoded in the specified <tt>charset</tt>.
     * The new buffer's <tt>readerIndex</tt> and <tt>writerIndex</tt> are
     * <tt>0</tt> and the length of the encoded string respectively.
     */
    static ChannelBufferPtr copiedBuffer(const std::string& string, int offset, int length) {
        return copiedBuffer(ByteOrder::BYTE_ORDER_BIG, ConstArray::fromString(string), offset, length);
    }

    /**
     * Creates a new buffer with the specified <tt>endianness</tt> whose
     * content is a subregion of the specified <tt>string</tt>.
     * The new buffer's <tt>readerIndex</tt> and <tt>writerIndex</tt> are
     * <tt>0</tt> and the length of the encoded string respectively.
     */
    static ChannelBufferPtr copiedBuffer(
        ByteOrder endianness, const std::string& string, int offset, int length) {
        if (length == 0) {
            return EMPTY_BUFFER;
        }
        return copiedBuffer(endianness, ConstArray::fromString(string), offset, length);
    }

    /**
     * Creates a new big-endian buffer whose content is the specified
     * <tt>string</tt> encoded in the specified <tt>charset</tt>.
     * The new buffer's <tt>readerIndex</tt> and <tt>writerIndex</tt> are
     * <tt>0</tt> and the length of the encoded string respectively.
     */
    static ChannelBufferPtr copiedBuffer(const std::wstring& string, const Charset& charset) {
        return copiedBuffer(ByteOrder::BYTE_ORDER_BIG, string, charset);
    }

    /**
     * Creates a new buffer with the specified <tt>endianness</tt> whose
     * content is the specified <tt>string</tt> encoded in the specified
     * <tt>charset</tt>.  The new buffer's <tt>readerIndex</tt> and
     * <tt>writerIndex</tt> are <tt>0</tt> and the length of the encoded string
     * respectively.
     */
    static ChannelBufferPtr copiedBuffer(ByteOrder endianness, const std::wstring& string, const Charset& charset);

    /**
     * Creates a new big-endian buffer whose content is a subregion of
     * the specified <tt>array</tt> encoded in the specified <tt>charset</tt>.
     * The new buffer's <tt>readerIndex</tt> and <tt>writerIndex</tt> are
     * <tt>0</tt> and the length of the encoded string respectively.
     */
    static ChannelBufferPtr copiedBuffer(const std::wstring& string, int offset, int length, const Charset& charset) {
        return copiedBuffer(ByteOrder::BYTE_ORDER_BIG, string, offset, length, charset);
    }

    /**
     * Creates a new big-endian buffer whose content is a subregion of
     * the specified <tt>string</tt> encoded in the specified <tt>charset</tt>.
     * The new buffer's <tt>readerIndex</tt> and <tt>writerIndex</tt> are
     * <tt>0</tt> and the length of the encoded string respectively.
     */
    static ChannelBufferPtr copiedBuffer(
        ByteOrder endianness, const std::wstring& string, int offset, int length, const Charset& charset);

    /**
     * Creates a read-only buffer which disallows any modification operations
     * on the specified <tt>buffer</tt>.  The new buffer has the same
     * <tt>readerIndex</tt> and <tt>writerIndex</tt> with the specified
     * <tt>buffer</tt>.
     */
    static ChannelBufferPtr unmodifiableBuffer(const ChannelBufferPtr& buffer);

    /**
     * Returns a <a href="http://en.wikipedia.org/wiki/Hex_dump">hex dump</a>
     * of the specified buffer's readable bytes.
     */
    static std::string hexDump(const ChannelBuffer& buffer) {
        return ChannelBuffers::hexDump(buffer, buffer.readerIndex(), buffer.readableBytes());
    }

    /**
     * Returns a <a href="http://en.wikipedia.org/wiki/Hex_dump">hex dump</a>
     * of the specified buffer's sub-region.
     */
    static std::string hexDump(const ChannelBuffer& buffer, int fromIndex, int length);

    /**
     * Calculates the hash code of the specified buffer.  This method is
     * useful when implementing a new buffer type.
     */
    static int hashCode(const ChannelBuffer& buffer);

    static bool equals(const ChannelBufferPtr& bufferA, const ChannelBufferPtr& bufferB);

    /**
     * Returns <tt>true</tt> if and only if the two specified buffers are
     * identical to each other as described in <tt>ChannelBuffer#equals(Object)</tt>.
     * This method is useful when implementing a new buffer type.
     */
    static bool equals(const ChannelBuffer& bufferA, const ChannelBuffer& bufferB);

    static int compare(const ChannelBufferPtr& bufferA, const ChannelBufferPtr& bufferB);

    /**
     * Compares the two specified buffers as described in {@link ChannelBuffer#compareTo(ChannelBuffer)}.
     * This method is useful when implementing a new buffer type.
     */
    static int compare(const ChannelBuffer& bufferA, const ChannelBuffer& bufferB);

    /**
     * The default implementation of {@link ChannelBuffer#indexOf(int, int, byte)}.
     * This method is useful when implementing a new buffer type.
     */
    static int indexOf(const ChannelBuffer& buffer, int fromIndex, int toIndex, boost::int8_t value);

    /**
     * The default implementation of {@link ChannelBuffer#indexOf(int, int, ChannelBufferIndexFinder)}.
     * This method is useful when implementing a new buffer type.
     */
    static int indexOf(const ChannelBuffer& buffer, int fromIndex, int toIndex, const ChannelBufferIndexFinder& indexFinder);

    /**
     * Toggles the endianness of the specified 16-bit short integer.
     */
    static boost::int16_t swapShort(boost::int16_t value);

    /**
     * Toggles the endianness of the specified 24-bit medium integer.
     */
    static int swapMedium(int value);

    /**
     * Toggles the endianness of the specified 32-bit integer.
     */
    static int swapInt(int value);

    /**
     * Toggles the endianness of the specified 64-bit long integer.
     */
    static boost::int64_t swapLong(boost::int64_t value);

private:
    static ChannelBufferPtr compositeBuffer(ByteOrder endianness, const std::vector<ChannelBufferPtr>& components);
    
    static int firstIndexOf(const ChannelBuffer& buffer, int fromIndex, int toIndex, boost::int8_t value);
    static int lastIndexOf(const ChannelBuffer& buffer, int fromIndex, int toIndex, boost::int8_t value);
    static int firstIndexOf(const ChannelBuffer& buffer, int fromIndex, int toIndex, const ChannelBufferIndexFinder& indexFinder);
    static int lastIndexOf(const ChannelBuffer& buffer, int fromIndex, int toIndex, const ChannelBufferIndexFinder& indexFinder);

    ChannelBuffers() {}
};

}}


#endif //#if !defined(CETTY_BUFFER_CHANNELBUFFERS_H)
