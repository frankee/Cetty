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

#include <boost/integer.hpp>

#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/buffer/BigEndianHeapChannelBuffer.h"
#include "cetty/buffer/LittleEndianHeapChannelBuffer.h"
#include "cetty/buffer/ReadOnlyChannelBuffer.h"
#include "cetty/buffer/DynamicChannelBuffer.h"
#include "cetty/buffer/SlicedChannelBuffer.h"
#include "cetty/buffer/TruncatedChannelBuffer.h"
#include "cetty/buffer/CompositeChannelBuffer.h"
#include "cetty/buffer/ChannelBufferFactory.h"

#include "cetty/buffer/ChannelBufferIndexFinder.h"

#include "cetty/util/Exception.h"

namespace cetty { namespace buffer {

using namespace cetty::util;

ChannelBufferPtr ChannelBuffers::EMPTY_BUFFER = 
                                    ChannelBufferPtr(new BigEndianHeapChannelBuffer(0));

static char HEXDUMP_TABLE[256 << 1];

static const char DIGITS[16] =
        {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

class HexDumpTableInitializer {
public:
    HexDumpTableInitializer() {
        for (int i = 0; i < 256; ++i) {
            HEXDUMP_TABLE[(i << 1) + 0] = DIGITS[(i >> 4) & 0x0F];
            HEXDUMP_TABLE[(i << 1) + 1] = DIGITS[(i >> 0) & 0x0F];
        }
    }
};

ChannelBufferPtr ChannelBuffers::buffer(ByteOrder endianness, int capacity) {
    if (endianness == ByteOrder::BYTE_ORDER_BIG) {
        if (capacity == 0) {
            return EMPTY_BUFFER;
        }
        return ChannelBufferPtr(new BigEndianHeapChannelBuffer(capacity));
    }
    else if (endianness == ByteOrder::BYTE_ORDER_LITTLE) {
        if (capacity == 0) {
            return EMPTY_BUFFER;
        }
        return ChannelBufferPtr(new LittleEndianHeapChannelBuffer(capacity));
    }

    return EMPTY_BUFFER;
}

ChannelBufferPtr ChannelBuffers::dynamicBuffer(ChannelBufferFactory& factory) {
    return ChannelBufferPtr(
            new DynamicChannelBuffer(factory.getDefaultOrder(), 256, factory));
}

ChannelBufferPtr ChannelBuffers::dynamicBuffer(ByteOrder endianness, int estimatedLength) {
    return ChannelBufferPtr(
            new DynamicChannelBuffer(endianness, estimatedLength));
}

ChannelBufferPtr ChannelBuffers::dynamicBuffer(int estimatedLength, ChannelBufferFactory& factory) {
    return ChannelBufferPtr(
            new DynamicChannelBuffer(factory.getDefaultOrder(), estimatedLength, factory));
}

ChannelBufferPtr ChannelBuffers::dynamicBuffer(ByteOrder endianness, int estimatedLength, ChannelBufferFactory& factory) {
    return ChannelBufferPtr(
            new DynamicChannelBuffer(endianness, estimatedLength, factory));
}

ChannelBufferPtr ChannelBuffers::wrappedBuffer(ByteOrder endianness, const Array& array) {
    if (endianness == ByteOrder::BYTE_ORDER_BIG) {
        if (array.length() == 0) {
            return EMPTY_BUFFER;
        }
        return ChannelBufferPtr(new BigEndianHeapChannelBuffer(array));
    }
    else if (endianness == ByteOrder::BYTE_ORDER_LITTLE) {
        if (array.length() == 0) {
            return EMPTY_BUFFER;
        }
        return ChannelBufferPtr(new LittleEndianHeapChannelBuffer(array));
    }

    return EMPTY_BUFFER;
}

ChannelBufferPtr ChannelBuffers::wrappedBuffer(ByteOrder endianness, const Array& array, int offset, int length) {
    if (length == 0) return EMPTY_BUFFER;
    return wrappedBuffer(endianness, Array(array.data(offset), length));
}

ChannelBufferPtr
ChannelBuffers::wrappedBuffer(ByteOrder endianness,
                              const Array& array0,
                              const Array& array1) {
    if (array0.empty()) {
      if (array1.empty()) {
          return EMPTY_BUFFER;
      }

      return wrappedBuffer(endianness, array1);
    }
    else {
        if (array1.empty()) {
            return wrappedBuffer(endianness, array0);
        }
        
        std::vector<Array> arrays;
        arrays.push_back(array0);
        arrays.push_back(array1);
        return wrappedBuffer(arrays);
    }
}

ChannelBufferPtr
ChannelBuffers::wrappedBuffer(ByteOrder endianness,
                              const Array& array0,
                              const Array& array1,
                              const Array& array2) {
    std::vector<Array> arrays;
    if (!array0.empty()) arrays.push_back(array0);
    if (!array1.empty()) arrays.push_back(array1);
    if (!array2.empty()) arrays.push_back(array2);

    if (arrays.empty()) return EMPTY_BUFFER;
    return wrappedBuffer(endianness, arrays);
}

ChannelBufferPtr
ChannelBuffers::wrappedBuffer(ByteOrder endianness,
                              const Array& array0,
                              const Array& array1,
                              const Array& array2,
                              const Array& array3) {
    std::vector<Array> arrays;
    if (!array0.empty()) arrays.push_back(array0);
    if (!array1.empty()) arrays.push_back(array1);
    if (!array2.empty()) arrays.push_back(array2);
    if (!array3.empty()) arrays.push_back(array3);

    if (arrays.empty()) return EMPTY_BUFFER;
    return wrappedBuffer(endianness, arrays);
}

ChannelBufferPtr ChannelBuffers::wrappedBuffer(ByteOrder endianness, std::vector<Array>& arrays) {
    std::vector<ChannelBufferPtr> buffs;

    for (size_t i = 0; i < arrays.size(); ++i) {
        if (!arrays[i].empty()) {
            buffs.push_back(ChannelBuffers::wrappedBuffer(endianness, arrays[i]));
        }
    }

    if (buffs.empty()) {
        return EMPTY_BUFFER;
    }
    else if (buffs.size() == 1) {
        return buffs[0];
    }

    return compositeBuffer(endianness, buffs);
}

#if 0
ChannelBufferPtr ChannelBuffers::wrappedBuffer(ChannelBufferPtr buffer0, ChannelBufferPtr buffer1, ...) {
    if (!buffer0) return EMPTY_BUFFER;

    va_list argp;
    int argno = 0;
    ChannelBufferPtr buff;
    std::vector<ChannelBufferPtr> buffs;
    if (buffer0->readable()) {
        buffs.push_back(buffer0);
    }

    if (!buffer1) {
        return wrappedBuffer(buffs);
    }
    if (buffer1->readable()) {
        buffs.push_back(buffer1);
    }

    va_start(argp, buffer1);
    do {
        ++argno;
        buff = va_arg(argp, ChannelBufferPtr);
        if (buff && buff->readable()) {
            buffs.push_back(buff);
        }
    }
    while(buff);

    return wrappedBuffer(buffs);
}
#endif

ChannelBufferPtr ChannelBuffers::wrappedBuffer(const std::vector<ChannelBufferPtr>& buffers) {
    if (buffers.size() == 0) return EMPTY_BUFFER;

    std::vector<ChannelBufferPtr> buffs;
    for (size_t i = 0; i < buffers.size(); ++i) {
        if (!buffers[i]->readable()) continue;

        if (!buffs.empty() && buffers[i]->order() != buffs[0]->order()) {
            // TODO: release resources.
            throw InvalidArgumentException("inconsistent byte order");
        }

        CompositeChannelBufferPtr compositeChannelBuff =
            boost::dynamic_pointer_cast<CompositeChannelBuffer>(buffers[i]);

        if (compositeChannelBuff) {
            // Expand nested composition.
            std::vector<ChannelBufferPtr> vec
                = compositeChannelBuff->decompose();
            
            buffs.insert(buffs.end(), vec.begin(), vec.end());
        }
        else {
            // An ordinary buffer (non-composite)
            buffs.push_back(buffers[i]->slice());
        }
    }

    if (buffs.empty()) {
        return EMPTY_BUFFER;
    }
    else if (buffs.size() == 1) {
        return buffs[0];
    }

    return compositeBuffer(buffs[0]->order(), buffs);
}

ChannelBufferPtr
ChannelBuffers::wrappedBuffer(const ChannelBufferPtr& buffer) {
    if (buffer && buffer->readable()) {
        return buffer->slice();
    }
    else {
        return EMPTY_BUFFER;
    }
}

ChannelBufferPtr
ChannelBuffers::wrappedBuffer(const ChannelBufferPtr& buffer0,
                              const ChannelBufferPtr& buffer1) {
    std::vector<ChannelBufferPtr> buffers;
    if (buffer0 && buffer0->readable()) buffers.push_back(buffer0);
    if (buffer1 && buffer1->readable()) buffers.push_back(buffer1);
    
    if (buffers.empty()) return EMPTY_BUFFER;
    return wrappedBuffer(buffers);
}

ChannelBufferPtr
ChannelBuffers::wrappedBuffer(const ChannelBufferPtr& buffer0,
                              const ChannelBufferPtr& buffer1,
                              const ChannelBufferPtr& buffer2) {
    std::vector<ChannelBufferPtr> buffers;
    if (buffer0 && buffer0->readable()) buffers.push_back(buffer0);
    if (buffer1 && buffer1->readable()) buffers.push_back(buffer1);
    if (buffer2 && buffer2->readable()) buffers.push_back(buffer2);

    if (buffers.empty()) return EMPTY_BUFFER;
    return wrappedBuffer(buffers);
}

ChannelBufferPtr
ChannelBuffers::wrappedBuffer(const ChannelBufferPtr& buffer0,
                              const ChannelBufferPtr& buffer1,
                              const ChannelBufferPtr& buffer2,
                              const ChannelBufferPtr& buffer3) {
    std::vector<ChannelBufferPtr> buffers;
    if (buffer0 && buffer0->readable()) buffers.push_back(buffer0);
    if (buffer1 && buffer1->readable()) buffers.push_back(buffer1);
    if (buffer2 && buffer2->readable()) buffers.push_back(buffer2);
    if (buffer3 && buffer3->readable()) buffers.push_back(buffer3);

    if (buffers.empty()) return EMPTY_BUFFER;
    return wrappedBuffer(buffers);
}

ChannelBufferPtr
ChannelBuffers::compositeBuffer(ByteOrder endianness,
                                const std::vector<ChannelBufferPtr>& components) {
    switch (components.size()) {
        case 0:
            return EMPTY_BUFFER;
        case 1:
            return components[0];
        default:
            return ChannelBufferPtr(
                new CompositeChannelBuffer(endianness, components));
    }
}

ChannelBufferPtr
ChannelBuffers::copiedBuffer(ByteOrder endianness, const ConstArray& array) {
    if (endianness == ByteOrder::BYTE_ORDER_BIG) {
        if (array.length() == 0) {
            return EMPTY_BUFFER;
        }
        return ChannelBufferPtr(new BigEndianHeapChannelBuffer(ConstArray::clone(array), true));
    }
    else if (endianness == ByteOrder::BYTE_ORDER_LITTLE) {
        if (array.length() == 0) {
            return EMPTY_BUFFER;
        }
        return ChannelBufferPtr(new LittleEndianHeapChannelBuffer(ConstArray::clone(array), true));
    }

    return EMPTY_BUFFER;
}

ChannelBufferPtr
ChannelBuffers::copiedBuffer(ByteOrder endianness,
                             const ConstArray& array,
                             int offset,
                             int length) {
    if (length <= 0 || array.empty() || offset >= array.length()) {
        return EMPTY_BUFFER;
    }
    return ChannelBuffers::wrappedBuffer(endianness, ConstArray::clone(array));
}

ChannelBufferPtr
ChannelBuffers::copiedBuffer(ByteOrder endianness,
                             const Array& array0,
                             const Array& array1) {
    if (array0.empty()) {
        if (array1.empty()) {
            return EMPTY_BUFFER;
        }
        else {
            return copiedBuffer(endianness, array1);
        }
    }
    else {
        if (array1.empty()) {
            return copiedBuffer(endianness, array0);
        }
        else {
            std::vector<Array> arrays;
            arrays.push_back(array0);
            arrays.push_back(array1);
            return copiedBuffer(endianness, arrays);
        }
    }
}

ChannelBufferPtr
ChannelBuffers::copiedBuffer(ByteOrder endianness,
                             const Array& array0,
                             const Array& array1,
                             const Array& array2) {
    std::vector<Array> arrays;
    if (!array0.empty()) arrays.push_back(array0);
    if (!array1.empty()) arrays.push_back(array1);
    if (!array2.empty()) arrays.push_back(array2);
    return copiedBuffer(endianness, arrays);
}

ChannelBufferPtr
ChannelBuffers::copiedBuffer(ByteOrder endianness,
                             const Array& array0,
                             const Array& array1,
                             const Array& array2,
                             const Array& array3) {
    std::vector<Array> arrays;
    if (!array0.empty()) arrays.push_back(array0);
    if (!array1.empty()) arrays.push_back(array1);
    if (!array2.empty()) arrays.push_back(array2);
    if (!array3.empty()) arrays.push_back(array3);
    return copiedBuffer(endianness, arrays);
}

ChannelBufferPtr
ChannelBuffers::copiedBuffer(ByteOrder endianness, const std::vector<Array>& arrays) {
    if (arrays.size() == 0) return EMPTY_BUFFER;
    if (arrays.size() == 1) {
        if (arrays[0].empty()) {
            return EMPTY_BUFFER;
        }

        return copiedBuffer(endianness, arrays[0]);
    }

    // Merge the specified arrays into one array.
    int length = 0;
    for (size_t i = 0; i < arrays.size(); ++i) {
        if (length + arrays[i].length() > std::numeric_limits<int>::max()) {
            throw InvalidArgumentException("The total length of the specified arrays is too big.");
        }
        length += arrays[i].length();
    }

    if (length == 0) {
        return EMPTY_BUFFER;
    }

    char* mergedBytes = new char[length];
    Array mergedArray(mergedBytes, length);
    for (size_t i = 0, j = 0; i < arrays.size(); ++i) {
        Array::copy(mergedArray, j, arrays[i], 0, arrays[i].length());
        j += arrays[i].length();
    }

    return wrappedBuffer(endianness, mergedArray);
}

ChannelBufferPtr
ChannelBuffers::copiedBuffer(const ChannelBufferPtr& buffer) {
    if (buffer && buffer->readable()) {
        return buffer->copy();
    }
    else {
        return EMPTY_BUFFER;
    }
}

ChannelBufferPtr
ChannelBuffers::copiedBuffer(const ChannelBufferPtr& buffer0,
                             const ChannelBufferPtr& buffer1) {
    if (buffer0 && buffer0->readable()) {
        if (buffer1 && buffer1->readable()) {
            if (buffer1->order() != buffer0->order()) {
                throw InvalidArgumentException("inconsistent byte order");
            }
            return wrappedBuffer(buffer0->copy(), buffer1->copy());
        }
        else {
            return buffer0->copy();
        }
    }
    else {
        if (buffer1 && buffer1->readable()) {
            return buffer1->copy();
        }
        else {
            return EMPTY_BUFFER;
        }
    }
}

ChannelBufferPtr
ChannelBuffers::copiedBuffer(const ChannelBufferPtr& buffer0,
                             const ChannelBufferPtr& buffer1,
                             const ChannelBufferPtr& buffer2) {
    std::vector<ChannelBufferPtr> buffers;
    if (buffer0 && buffer0->readable()) buffers.push_back(buffer0->copy());
    if (buffer1 && buffer1->readable()) {
        if (buffers.size() > 1 && buffer1->order() != buffers[0]->order()) {
            throw InvalidArgumentException("inconsistent byte order");
        }
        buffers.push_back(buffer1->copy());
    }
    if (buffer2 && buffer2->readable()) {
        if (buffers.size() > 1 && buffer2->order() != buffers[0]->order()) {
            throw InvalidArgumentException("inconsistent byte order");
        }
        buffers.push_back(buffer2->copy());
    }

    if (buffers.empty()) {
        return EMPTY_BUFFER;
    }
    else if (buffers.size() == 1) {
        return buffers.front();
    }
    else {
        return wrappedBuffer(buffers);
    }
}

ChannelBufferPtr
ChannelBuffers::copiedBuffer(const ChannelBufferPtr& buffer0,
                             const ChannelBufferPtr& buffer1,
                             const ChannelBufferPtr& buffer2,
                             const ChannelBufferPtr& buffer3) {
    std::vector<ChannelBufferPtr> buffers;
    if (buffer0 && buffer0->readable()) {
        buffers.push_back(buffer0->copy());
    }
    if (buffer1 && buffer1->readable()) {
        if (buffers.size() > 1 && buffer1->order() != buffers[0]->order()) {
            throw InvalidArgumentException("inconsistent byte order");
        }
        buffers.push_back(buffer1->copy());
    }
    if (buffer2 && buffer2->readable()) {
        if (buffers.size() > 1 && buffer2->order() != buffers[0]->order()) {
            throw InvalidArgumentException("inconsistent byte order");
        }
        buffers.push_back(buffer2->copy());
    }
    if (buffer3 && buffer3->readable()) {
        if (buffers.size() > 1 && buffer3->order() != buffers[0]->order()) {
            throw InvalidArgumentException("inconsistent byte order");
        }
        buffers.push_back(buffer3->copy());
    }

    if (buffers.empty()) {
        return EMPTY_BUFFER;
    }
    else if (buffers.size() == 1) {
        return buffers.front();
    }
    else {
        return wrappedBuffer(buffers);
    }
}

ChannelBufferPtr
ChannelBuffers::copiedBuffer(const std::vector<ChannelBufferPtr>& buffers) {
    if (buffers.size() == 0) return EMPTY_BUFFER;

    std::vector<ChannelBufferPtr> buffs;
    for (size_t i = 0; i < buffers.size(); ++i) {
        if (!buffs.empty() && buffers[i]->order() != buffs[0]->order()) {
            throw InvalidArgumentException("inconsistent byte order");
        }

        buffs.push_back(ChannelBuffers::copiedBuffer(buffers[i]));
    }

    if (buffs.empty()) {
        return EMPTY_BUFFER;
    }
    else if (buffs.size() == 1) {
        return buffs[0];
    }

    return wrappedBuffer(buffs);
}

ChannelBufferPtr
ChannelBuffers::copiedBuffer(ByteOrder endianness, const std::wstring& string, const Charset& charset) {
    //TODO
    return EMPTY_BUFFER;
}

ChannelBufferPtr
ChannelBuffers::copiedBuffer(ByteOrder endianness, const std::wstring& string, int offset, int length, const Charset& charset) {
    if (length == 0) {
        return EMPTY_BUFFER;
    }
    //TODO
    return EMPTY_BUFFER;
}

std::string ChannelBuffers::hexDump(const ChannelBuffer& buffer, int fromIndex, int length) {
    static HexDumpTableInitializer initializer;

    std::string buf;
    if (length < 0) {
        throw InvalidArgumentException("length is less than zero.");
    }
    if (length == 0) {
        return buf;
    }

    int endIndex = fromIndex + length;
    buf.reserve(length);

    int srcIdx = fromIndex;
    int dstIdx = 0;

    for (; srcIdx < endIndex; ++srcIdx, dstIdx += 2) {
        buf.push_back(HEXDUMP_TABLE[(buffer.getUnsignedByte(srcIdx)) << 1]);
        buf.push_back(HEXDUMP_TABLE[((buffer.getUnsignedByte(srcIdx)) << 1) + 1]);
    }

    return buf;
}

ChannelBufferPtr ChannelBuffers::unmodifiableBuffer(const ChannelBufferPtr& buffer) {
    if (!buffer) {
        return ChannelBufferPtr(new ReadOnlyChannelBuffer(EMPTY_BUFFER));
    }

    ReadOnlyChannelBuffer* buf =
        dynamic_cast<ReadOnlyChannelBuffer*>(buffer.get());

    if (buf) {
        return ChannelBufferPtr(new ReadOnlyChannelBuffer(buf->unwrap()));
    }

    return ChannelBufferPtr(new ReadOnlyChannelBuffer(buffer));
}

int ChannelBuffers::firstIndexOf( const ChannelBuffer& buffer, int fromIndex, int toIndex, const ChannelBufferIndexFinder& indexFinder )
{
    fromIndex = std::max(fromIndex, 0);
    if (fromIndex >= toIndex || buffer.capacity() == 0) {
        return -1;
    }

    for (int i = fromIndex; i < toIndex; i ++) {
        if (indexFinder.find(buffer, i)) {
            return i;
        }
    }

    return -1;
}

int ChannelBuffers::firstIndexOf( const ChannelBuffer& buffer, int fromIndex, int toIndex, boost::int8_t value )
{
    fromIndex = std::max(fromIndex, 0);
    if (fromIndex >= toIndex || buffer.capacity() == 0) {
        return -1;
    }

    for (int i = fromIndex; i < toIndex; ++i) {
        if (buffer.getByte(i) == value) {
            return i;
        }
    }

    return -1;
}

int ChannelBuffers::lastIndexOf( const ChannelBuffer& buffer, int fromIndex, int toIndex, const ChannelBufferIndexFinder& indexFinder )
{
    fromIndex = std::min(fromIndex, buffer.capacity());
    if (fromIndex < 0 || buffer.capacity() == 0) {
        return -1;
    }

    for (int i = fromIndex - 1; i >= toIndex; i --) {
        if (indexFinder.find(buffer, i)) {
            return i;
        }
    }

    return -1;
}

int ChannelBuffers::lastIndexOf(const ChannelBuffer& buffer, int fromIndex, int toIndex, boost::int8_t value) {
    fromIndex = std::min(fromIndex, buffer.capacity());
    if (fromIndex < 0 || buffer.capacity() == 0) {
        return -1;
    }

    for (int i = fromIndex - 1; i >= toIndex; --i) {
        if (buffer.getByte(i) == value) {
            return i;
        }
    }

    return -1;
}

int ChannelBuffers::hashCode(const ChannelBuffer& buffer) {
    int aLen = buffer.readableBytes();
    int intCount = aLen >> 2;
    int byteCount = aLen & 3;

    int hashCode = 1;
    int arrayIndex = buffer.readerIndex();
    if (buffer.order() == ByteOrder::BYTE_ORDER_BIG) {
        for (int i = intCount; i > 0; --i) {
            hashCode = 31 * hashCode + buffer.getInt(arrayIndex);
            arrayIndex += 4;
        }
    }
    else {
        for (int i = intCount; i > 0; --i) {
            hashCode = 31 * hashCode + swapInt(buffer.getInt(arrayIndex));
            arrayIndex += 4;
        }
    }

    for (int i = byteCount; i > 0; --i) {
        hashCode = 31 * hashCode + buffer.getByte(arrayIndex++);
    }

    if (hashCode == 0) {
        hashCode = 1;
    }

    return hashCode;
}

bool ChannelBuffers::equals(const ChannelBufferPtr& bufferA, const ChannelBufferPtr& bufferB) {
    if (!bufferA || !bufferB) return false;
    return equals(*bufferA, *bufferB);
}

bool ChannelBuffers::equals(const ChannelBuffer& bufferA, const ChannelBuffer& bufferB) {
    int aLen = bufferA.readableBytes();
    if (aLen != bufferB.readableBytes()) {
        return false;
    }

    int longCount = aLen >> 3;
    int byteCount = aLen & 7;

    int aIndex = bufferA.readerIndex();
    int bIndex = bufferB.readerIndex();

    if (bufferA.order() == bufferB.order()) {
        for (int i = longCount; i > 0; --i) {
            if (bufferA.getLong(aIndex) != bufferB.getLong(bIndex)) {
                return false;
            }
            aIndex += 8;
            bIndex += 8;
        }
    }
    else {
        for (int i = longCount; i > 0; --i) {
            if (bufferA.getLong(aIndex) != swapLong(bufferB.getLong(bIndex))) {
                return false;
            }
            aIndex += 8;
            bIndex += 8;
        }
    }

    for (int i = byteCount; i > 0; --i) {
        if (bufferA.getByte(aIndex) != bufferB.getByte(bIndex)) {
            return false;
        }
        ++aIndex;
        ++bIndex;
    }

    return true;
}

int ChannelBuffers::compare(const ChannelBufferPtr& bufferA, const ChannelBufferPtr& bufferB) {
    if (!bufferA) {
        if (!bufferB) {
            return 0;
        }
        return 1;
    }
    if (!bufferB) {
        return -1;
    }

    return compare(*bufferA, *bufferB);
}

int ChannelBuffers::compare(const ChannelBuffer& bufferA, const ChannelBuffer& bufferB) {
    int aLen = bufferA.readableBytes();
    int bLen = bufferB.readableBytes();
    int minLength = aLen < bLen ? aLen : bLen/*std::min(aLen, bLen)*/;
    int uintCount = (minLength >> 2);
    int byteCount = (minLength & 3);

    int aIndex = bufferA.readerIndex();
    int bIndex = bufferB.readerIndex();

    if (bufferA.order() == bufferB.order()) {
        for (int i = uintCount; i > 0; --i) {
            boost::uint32_t va = bufferA.getUnsignedInt(aIndex);
            boost::uint32_t vb = bufferB.getUnsignedInt(bIndex);
            if (va > vb) {
                return 1;
            }
            else if (va < vb) {
                return -1;
            }
            aIndex += 4;
            bIndex += 4;
        }
    }
    else {
        for (int i = uintCount; i > 0; --i) {
            boost::uint32_t va = bufferA.getUnsignedInt(aIndex);
            boost::uint32_t vb = swapInt(bufferB.getInt(bIndex)) & 0xFFFFFFFFL;
            if (va > vb) {
                return 1;
            }
            else if (va < vb) {
                return -1;
            }
            aIndex += 4;
            bIndex += 4;
        }
    }

    for (int i = byteCount; i > 0; --i) {
        boost::int8_t va = bufferA.getByte(aIndex);
        boost::int8_t vb = bufferB.getByte(bIndex);
        if (va > vb) {
            return 1;
        }
        else if (va < vb) {
            return -1;
        }
        ++aIndex;
        ++bIndex;
    }

    return aLen - bLen;
}

int ChannelBuffers::indexOf(const ChannelBuffer& buffer, int fromIndex, int toIndex, boost::int8_t value) {
    if (fromIndex <= toIndex) {
        return firstIndexOf(buffer, fromIndex, toIndex, value);
    }
    else {
        return lastIndexOf(buffer, fromIndex, toIndex, value);
    }
}

int ChannelBuffers::indexOf(const ChannelBuffer& buffer, int fromIndex, int toIndex, const ChannelBufferIndexFinder& indexFinder) {
    if (fromIndex <= toIndex) {
        return firstIndexOf(buffer, fromIndex, toIndex, indexFinder);
    }
    else {
        return lastIndexOf(buffer, fromIndex, toIndex, indexFinder);
    }
}

boost::int16_t ChannelBuffers::swapShort(boost::int16_t value) {
    return ((value << 8) | ((value >> 8) & 0xff));
}

int ChannelBuffers::swapMedium(int value) {
    return ((value << 16) & 0xff0000) | (value & 0xff00) | ((value >> 16) & 0xff);
}

int ChannelBuffers::swapInt(int value) {
    return (swapShort((boost::int16_t)value) <<  16) |
        (swapShort((boost::int16_t)(value >> 16)) & 0xffff);
}

boost::int64_t ChannelBuffers::swapLong(boost::int64_t value) {
    return ((boost::int64_t)(swapInt((boost::int32_t)value)) << 32) |
        (swapInt((boost::int32_t)(value >> 32)) & 0xffffffffL);
}

}}
