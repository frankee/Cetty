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

#include "cetty/buffer/AbstractChannelBuffer.h"

#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/buffer/ChannelBufferFactory.h"
#include "cetty/util/Exception.h"
#include "cetty/util/Integer.h"
#include "cetty/util/Charset.h"

namespace cetty { namespace buffer { 

using namespace cetty::util;

int AbstractChannelBuffer::readerIndex() const {
    return this->readerIdx;
}

void AbstractChannelBuffer::readerIndex(int readerIdx) {
    if (readerIdx < 0 || readerIdx > this->writerIdx) {
        throw RangeException("readerIndex");
    }
    this->readerIdx = readerIdx;
}

void AbstractChannelBuffer::offsetReaderIndex(int offset) {
    int newReaderIdx = readerIdx + offset;
    if (newReaderIdx < 0 || newReaderIdx > writerIdx) {
        throw RangeException("readerIndex");
    }
    readerIdx = newReaderIdx;
}

int AbstractChannelBuffer::writerIndex() const {
    return writerIdx;
}

void AbstractChannelBuffer::writerIndex(int writerIdx) {
    if (writerIdx < readerIdx || writerIdx > capacity()) {
        throw RangeException("writerIndex");
    }
    this->writerIdx = writerIdx;
}

void AbstractChannelBuffer::offsetWriterIndex(int offset) {
    int newWriterIdx = this->writerIdx + offset;
    if (newWriterIdx < readerIdx || newWriterIdx > capacity()) {
        throw RangeException("writerIndex");
    }
    this->writerIdx = newWriterIdx;
}

void AbstractChannelBuffer::setIndex(int readerIndex, int writerIndex) {
    if (readerIndex < 0 || readerIndex > writerIndex || writerIndex > capacity()) {
        throw RangeException("readerIndex or writerIndex out of range");
    }
    this->readerIdx = readerIndex;
    this->writerIdx = writerIndex;
}

void AbstractChannelBuffer::clear() {
    readerIdx = writerIdx = 0;
}

bool AbstractChannelBuffer::readable() const {
    return readableBytes() > 0;
}

bool AbstractChannelBuffer::writable() const {
    return writableBytes() > 0;
}

int AbstractChannelBuffer::readableBytes() const {
    return writerIdx - readerIdx;
}

int AbstractChannelBuffer::writableBytes() const {
    return capacity() - writerIdx;
}

void AbstractChannelBuffer::markReaderIndex() {
    this->markedReaderIndex = readerIdx;
}

void AbstractChannelBuffer::resetReaderIndex() {
    readerIndex(this->markedReaderIndex);
}

void AbstractChannelBuffer::markWriterIndex() {
    this->markedWriterIndex = writerIdx;
}

void AbstractChannelBuffer::resetWriterIndex() {
    writerIdx = this->markedWriterIndex;
}

void AbstractChannelBuffer::discardReadBytes() {
    if (readerIdx == 0) return;

    if (readerIdx == writerIdx) {
        readerIdx = writerIdx = 0;
        return;
    }

    setBytes(0, *this, readerIdx, writerIdx - readerIdx);

    writerIdx -= readerIdx;
    this->markedReaderIndex = this->markedReaderIndex - readerIdx > 0 ? this->markedReaderIndex - readerIdx : 0;
    this->markedWriterIndex = this->markedWriterIndex - readerIdx > 0 ? this->markedWriterIndex - readerIdx : 0;
    readerIdx = 0;
}

void AbstractChannelBuffer::ensureWritableBytes(int wBytes) {
    if (wBytes > writableBytes()) {
        throw RangeException("has no enough capacity to write");
    }
}

boost::uint8_t AbstractChannelBuffer::getUnsignedByte(int index) const {
    return (boost::uint8_t) (getByte(index) & 0xFF);
}

boost::uint16_t AbstractChannelBuffer::getUnsignedShort(int index) const {
    return getShort(index) & 0xFFFF;
}

boost::int32_t AbstractChannelBuffer::getMedium(int index) const {
    int value = getUnsignedMedium(index);
    if ((value & 0x800000) != 0) {
        value |= 0xff000000;
    }
    return value;
}

boost::uint32_t AbstractChannelBuffer::getUnsignedInt(int index) const {
    return getInt(index) & 0xFFFFFFFF;
}

wchar_t AbstractChannelBuffer::getChar(int index) const {
    return (wchar_t) getShort(index);
}

float AbstractChannelBuffer::getFloat(int index) const {
    //TODO
    return 0;
    //return Float.intBitsToFloat(getInt(index));
}

double AbstractChannelBuffer::getDouble(int index) const {
    return 0;
    //return Double.longBitsToDouble(getLong(index));
}

void AbstractChannelBuffer::getBytes(int index, const Array& dst) const {
    getBytes(index, dst, 0, dst.length());
}

void AbstractChannelBuffer::getBytes(int index, std::string& dst, int length) const {
    dst.resize(length);
    getBytes(index, Array::fromString(dst));
}

void AbstractChannelBuffer::getBytes(int index, std::string& dst, int dstIndex, int length) const {
    dst.resize(dstIndex + length);
    getBytes(index, Array::fromString(dst), dstIndex, length);
}

void AbstractChannelBuffer::getBytes(int index, ChannelBuffer& dst) const {
    getBytes(index, dst, dst.writableBytes());
}

void AbstractChannelBuffer::getBytes(int index, ChannelBuffer& dst, int length) const {
    if (length > dst.writableBytes()) {
        throw RangeException("has no enough capacity to write");
    }
    getBytes(index, dst, dst.writerIndex(), length);
    dst.writerIndex(dst.writerIndex() + length);
}

void AbstractChannelBuffer::setChar(int index, int value) {
    setShort(index, value);
}

void AbstractChannelBuffer::setFloat(int index, float value) {
    //TODO
    //setInt(index, Float.floatToRawIntBits(value));
}

void AbstractChannelBuffer::setDouble(int index, double value) {
    //TODO
    //setLong(index, Double.doubleToRawLongBits(value));
}

void AbstractChannelBuffer::setBytes(int index, const ConstArray& src) {
    setBytes(index, src, 0, src.length());
}

void AbstractChannelBuffer::setBytes(int index, const std::string& src) {
    setBytes(index, ConstArray::fromString(src));
}

void AbstractChannelBuffer::setBytes(int index, const std::string& src, int srcIndex, int length) {
    setBytes(index, ConstArray::fromString(src), srcIndex, length);
}

void AbstractChannelBuffer::setBytes(int index, ChannelBuffer& src) {
    setBytes(index, src, src.readableBytes());
}

void AbstractChannelBuffer::setBytes(int index, ChannelBuffer& src, int length) {
    if (length > src.readableBytes()) {
        throw RangeException("exceed readable bytes");
    }
    setBytes(index, src, src.readerIndex(), length);
    src.readerIndex(src.readerIndex() + length);
}

void AbstractChannelBuffer::setZero(int index, int length) {
    if (length == 0) {
        return;
    }
    if (length < 0) {
        throw InvalidArgumentException("length must be 0 or greater than 0.");
    }

    //if length out_of_range, may cause out_of_range exception or just reduce the length.
    //int leftBtyes = capacity() - index;
    //if (length > leftBtyes) length = leftBtyes;

    int nLong = length >> 3;
    int nBytes = length & 7;
    for (int i = nLong; i > 0; --i) {
        setLong(index, 0);
        index += 8;
    }
    if (nBytes == 4) {
        setInt(index, 0);
    }
    else if (nBytes < 4) {
        for (int i = nBytes; i > 0; --i) {
            setByte(index, 0);
            ++index;
        }
    } else {
        setInt(index, 0);
        index += 4;
        for (int i = nBytes - 4; i > 0; --i) {
            setByte(index, 0);
            ++index;
        }
    }
}

boost::int8_t AbstractChannelBuffer::readByte() {
    if (readerIdx == writerIdx) {
        throw RangeException("no data to read");
    }
    return getByte(readerIdx++);
}

boost::uint8_t AbstractChannelBuffer::readUnsignedByte() {
    return (boost::uint8_t) (readByte() & 0xFF);
}

boost::int16_t AbstractChannelBuffer::readShort() {
    checkReadableBytes(2);
    boost::int16_t v = getShort(readerIdx);
    readerIdx += 2;
    return v;
}

boost::uint16_t AbstractChannelBuffer::readUnsignedShort() {
    return boost::uint16_t(readShort() & 0xFFFF);
}

boost::int32_t AbstractChannelBuffer::readMedium() {
    int value = readUnsignedMedium();
    if ((value & 0x800000) != 0) {
        value |= 0xff000000;
    }
    return value;
}

boost::int32_t AbstractChannelBuffer::readUnsignedMedium() {
    checkReadableBytes(3);
    int v = getUnsignedMedium(readerIdx);
    readerIdx += 3;
    return v;
}

boost::int32_t AbstractChannelBuffer::readInt() {
    checkReadableBytes(4);
    boost::int32_t v = getInt(readerIdx);
    readerIdx += 4;
    return v;
}

boost::uint32_t AbstractChannelBuffer::readUnsignedInt() {
    return boost::uint32_t(readInt() & 0xFFFFFFFF);
}

boost::int64_t AbstractChannelBuffer::readLong() {
    checkReadableBytes(8);
    boost::int64_t v = getLong(readerIdx);
    readerIdx += 8;
    return v;
}

wchar_t AbstractChannelBuffer::readChar() {
    return (wchar_t) readShort();
}

float AbstractChannelBuffer::readFloat() {
    return 0;
    // TODO
    //return Float.intBitsToFloat(readInt());
}

double AbstractChannelBuffer::readDouble() {
    return 0;
    // TODO
    //return Double.longBitsToDouble(readLong());
}

ChannelBufferPtr AbstractChannelBuffer::readBytes() {
    return readBytes(readableBytes());
}

ChannelBufferPtr AbstractChannelBuffer::readBytes(int length) {
    if (0 == length) {
        return ChannelBuffers::EMPTY_BUFFER;
    }

    checkReadableBytes(length);

    ChannelBufferPtr buf = factory().getBuffer(order(), length);
    buf->writeBytes(*this, readerIdx, length);
    readerIdx += length;
    if (readerIdx == writerIdx) {
        readerIdx = writerIdx = 0;
    }

    return buf;
}

void AbstractChannelBuffer::readBytes(const Array& dst, int dstIndex, int length) {
    if (0 == length) return;

    checkReadableBytes(length);
    getBytes(readerIdx, dst, dstIndex, length);
    readerIdx += length;
}

void AbstractChannelBuffer::readBytes(const Array& dst) {
    if (dst.empty()) return;

    readBytes(dst, 0, dst.length());
}

void AbstractChannelBuffer::readBytes(std::string& dst) {
    dst.resize(readableBytes());
    readBytes(Array::fromString(dst));
}

void AbstractChannelBuffer::readBytes(std::string& dst, int length) {
    if (0 == length) return;

    dst.resize(length);
    readBytes(Array::fromString(dst), 0, length);
}

void AbstractChannelBuffer::readBytes(std::string& dst, int dstIndex, int length) {
    if (0 == length) return;

    dst.resize(dstIndex + length);
    readBytes(Array::fromString(dst), dstIndex, length);
}

void AbstractChannelBuffer::readBytes(ChannelBuffer& dst) {
    readBytes(dst, dst.writableBytes());
}

void AbstractChannelBuffer::readBytes(ChannelBuffer& dst, int length) {
    if (length > dst.writableBytes()) {
        throw RangeException("readBytes out of range");
    }
    readBytes(dst, dst.writerIndex(), length);
    dst.writerIndex(dst.writerIndex() + length);
}

void AbstractChannelBuffer::readBytes(ChannelBuffer& dst, int dstIndex, int length) {
    checkReadableBytes(length);
    getBytes(readerIdx, dst, dstIndex, length);
    readerIdx += length;
}

void AbstractChannelBuffer::readBytes(OutputStream& out, int length) {
    checkReadableBytes(length);
    getBytes(readerIdx, out, length);
    readerIdx += length;
}

ChannelBufferPtr AbstractChannelBuffer::readSlice() {
    return readSlice(readableBytes());
}

cetty::buffer::ChannelBufferPtr AbstractChannelBuffer::readSlice(int length) {
    ChannelBufferPtr sliceBuffer = slice(readerIdx, length);
    readerIdx += length;
    return sliceBuffer;
}

void AbstractChannelBuffer::skipBytes(int length) {
    int newReaderIndex = readerIdx + length;
    if (newReaderIndex > writerIdx) {
        throw RangeException("");
    }
    readerIdx = newReaderIndex;
}

void AbstractChannelBuffer::writeByte(int value) {
    setByte(writerIdx++, value);
}

void AbstractChannelBuffer::writeShort(int value) {
    setShort(writerIdx, value);
    writerIdx += 2;
}

void AbstractChannelBuffer::writeMedium(int value) {
    setMedium(writerIdx, value);
    writerIdx += 3;
}

void AbstractChannelBuffer::writeInt(int value) {
    setInt(writerIdx, value);
    writerIdx += 4;
}

void AbstractChannelBuffer::writeLong(boost::int64_t value) {
    setLong(writerIdx, value);
    writerIdx += 8;
}

void AbstractChannelBuffer::writeChar(int value) {
    writeShort(value);
}

void AbstractChannelBuffer::writeFloat(float value) {
    //TODO
    //writeInt(Float.floatToRawIntBits(value));
}

void AbstractChannelBuffer::writeDouble(double value) {
    //writeLong(Double.doubleToRawLongBits(value));
}

void AbstractChannelBuffer::writeBytes(const ConstArray& src, int srcIndex, int length) {
    setBytes(this->writerIdx, src, srcIndex, length);
    this->writerIdx += length;
}

void AbstractChannelBuffer::writeBytes(const ConstArray& src) {
    writeBytes(src, 0, src.length());
}

void AbstractChannelBuffer::writeBytes(const std::string& src) {
    writeBytes(ConstArray::fromString(src), 0, (int)src.length());
}

void AbstractChannelBuffer::writeBytes(const std::string& src, int srcIndex, int length) {
    writeBytes(ConstArray::fromString(src), srcIndex, length);
}

void AbstractChannelBuffer::writeBytes(ChannelBuffer& src) {
    writeBytes(src, src.readableBytes());
}

void AbstractChannelBuffer::writeBytes(ChannelBuffer& src, int length) {
    if (length > src.readableBytes()) {
        throw RangeException("no data to read");
    }
    writeBytes(src, src.readerIndex(), length);
    src.readerIndex(src.readerIndex() + length);
}

void AbstractChannelBuffer::writeBytes(const ChannelBuffer& src, int srcIndex, int length) {
    setBytes(writerIdx, src, srcIndex, length);
    writerIdx += length;
}

int AbstractChannelBuffer::writeBytes(InputStream& in, int length) {
    int writtenBytes = setBytes(writerIdx, in, length);
    if (writtenBytes > 0) {
        writerIdx += writtenBytes;
    }
    return writtenBytes;
}

void AbstractChannelBuffer::writeZero(int length) {
    if (length == 0) {
        return;
    }
    if (length < 0) {
        throw InvalidArgumentException("length must be 0 or greater than 0.");
    }

    //if length more than writableBytes(), may cause out_of_range exception
    //or auto reduce the length to writableBytes().
    //if (length > writableBytes()) length = writableBytes();

    int nLong = length >> 3;
    int nBytes = length & 7;
    for (int i = nLong; i > 0; --i) {
        writeLong(0);
    }
    if (nBytes == 4) {
        writeInt(0);
    } else if (nBytes < 4) {
        for (int i = nBytes; i > 0; --i) {
            writeByte(0);
        }
    } else {
        writeInt(0);
        for (int i = nBytes - 4; i > 0; --i) {
            writeByte(0);
        }
    }
}

cetty::buffer::ChannelBufferPtr AbstractChannelBuffer::copy() const {
    return copy(readerIdx, readableBytes());
}

cetty::buffer::ChannelBufferPtr AbstractChannelBuffer::slice() {
    return slice(readerIdx, readableBytes());
}

std::string AbstractChannelBuffer::toString(const Charset& charset) {
    return toString(readerIndex(), readableBytes(), charset);
}

std::string AbstractChannelBuffer::toString(int index, int length, const Charset& charset) {
    if (length == 0) {
        return "";
    }

    std::string str;
    str.reserve(length);

    for (int i = 0; i < length; ++i) {
        str.push_back(getByte(index + i));
    }

    return str;
}

std::wstring AbstractChannelBuffer::toWideString(const Charset& charset) {
    return toWideString(readerIndex(), readableBytes(), charset);
}

std::wstring AbstractChannelBuffer::toWideString(int index, int length, const Charset& charset) {
    if (length == 0) {
        return L"";
    }

    // TODO
    return L"";

//     return ChannelBuffers::decodeString(
//         toByteBuffer(index, length), charset);
}

int AbstractChannelBuffer::indexOf(int fromIndex, int toIndex, boost::int8_t value) const {
    return ChannelBuffers::indexOf(*this, fromIndex, toIndex, value);
}

int AbstractChannelBuffer::indexOf(int fromIndex, int toIndex, const ChannelBufferIndexFinder& indexFinder) const {
    return ChannelBuffers::indexOf(*this, fromIndex, toIndex, indexFinder);
}

int AbstractChannelBuffer::bytesBefore(boost::int8_t value) const {
    return bytesBefore(readerIndex(), readableBytes(), value);
}

int AbstractChannelBuffer::bytesBefore(const ChannelBufferIndexFinder& indexFinder) const {
    return bytesBefore(readerIndex(), readableBytes(), indexFinder);
}

int AbstractChannelBuffer::bytesBefore(int length, boost::int8_t value) const {
    checkReadableBytes(length);
    return bytesBefore(readerIndex(), length, value);
}

int AbstractChannelBuffer::bytesBefore(int length, const ChannelBufferIndexFinder& indexFinder) const {
    checkReadableBytes(length);
    return bytesBefore(readerIndex(), length, indexFinder);
}

int AbstractChannelBuffer::bytesBefore(int index, int length, boost::int8_t value) const {
    int endIndex = indexOf(index, index + length, value);
    if (endIndex < 0) {
        return -1;
    }
    return endIndex - index;
}

int AbstractChannelBuffer::bytesBefore(int index, int length, const ChannelBufferIndexFinder& indexFinder) const {
    int endIndex = indexOf(index, index + length, indexFinder);
    if (endIndex < 0) {
        return -1;
    }
    return endIndex - index;
}

int AbstractChannelBuffer::hashCode() const {
    return ChannelBuffers::hashCode(*this);
}

bool AbstractChannelBuffer::equals(const ChannelBuffer& cb) const {
    return ChannelBuffers::equals(*this, cb);
}

int AbstractChannelBuffer::compareTo(const ChannelBuffer& that) const {
    return ChannelBuffers::compare(*this, that);
}

void AbstractChannelBuffer::checkReadableBytes(int minimumReadableBytes) const {
    if (readableBytes() < minimumReadableBytes) {
        throw RangeException("no data to read");
    }
}

std::string AbstractChannelBuffer::toString() const {
    std::string buf;
    buf = typeid(this).name();

    buf.append("(ridx=");
    Integer::append(buf, readerIdx);
    buf.append(", widx=");
    Integer::append(buf, writerIdx);
    buf.append(", cap=");
    Integer::append(buf, capacity());
    buf.append(")");

    return buf;
}

}}

