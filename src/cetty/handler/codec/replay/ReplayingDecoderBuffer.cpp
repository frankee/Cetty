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

#include "cetty/handler/codec/replay/ReplayingDecoderBuffer.h"
#include "cetty/handler/codec/replay/UnreplayableOperationException.h"

#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/util/Integer.h"

namespace cetty { namespace handler { namespace codec { namespace replay { 

using namespace cetty::buffer;
using namespace cetty::util;

int ReplayingDecoderBuffer::capacity() const {
    if (terminated) {
        return buffer->capacity();
    }
    else {
        return Integer::MAX_VALUE;
    }
}

bool ReplayingDecoderBuffer::hasArray() const {
    return buffer->hasArray();
}

const Array& ReplayingDecoderBuffer::array() {
    return buffer->array();
}

ConstArray ReplayingDecoderBuffer::array() const {
    return buffer->array();
}

int ReplayingDecoderBuffer::arrayOffset() const {
    return buffer->arrayOffset();
}

void ReplayingDecoderBuffer::clear() {
    throw UnreplayableOperationException();
}

bool ReplayingDecoderBuffer::equals(const ChannelBuffer& buffer) const {
    return this == &buffer;
}

int ReplayingDecoderBuffer::compareTo(const ChannelBuffer& buffer) const {
    throw UnreplayableOperationException();
}

cetty::buffer::ChannelBufferPtr ReplayingDecoderBuffer::copy() const {
    throw UnreplayableOperationException();
}

cetty::buffer::ChannelBufferPtr ReplayingDecoderBuffer::copy(int index, int length) const {
    if (checkIndex(index, length)) {
        return buffer->copy(index, length);
    }
    return ChannelBuffers::EMPTY_BUFFER;
}

void ReplayingDecoderBuffer::discardReadBytes() {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::ensureWritableBytes(int writableBytes) {
    throw UnreplayableOperationException();
}

cetty::buffer::ChannelBufferPtr ReplayingDecoderBuffer::duplicate() {
    throw UnreplayableOperationException();
}

boost::int8_t ReplayingDecoderBuffer::getByte(int index) const {
    if (checkIndex(index)) {
        return buffer->getByte(index);
    }
    return 0;
}

boost::uint8_t ReplayingDecoderBuffer::getUnsignedByte(int index) const {
    if (checkIndex(index)) {
        return buffer->getUnsignedByte(index);
    }
    return 0;
}

void ReplayingDecoderBuffer::getBytes(int index, const Array& dst, int dstIndex, int length) const {
    if (checkIndex(index, length)) {
        buffer->getBytes(index, dst, dstIndex, length);
    }
}

void ReplayingDecoderBuffer::getBytes(int index, const Array& dst) const {
    if (checkIndex(index, dst.length())) {
        buffer->getBytes(index, dst);
    }
}

void ReplayingDecoderBuffer::getBytes(int index, std::string& dst, int length) const {
    if (checkIndex(index, length)) {
        buffer->getBytes(index, dst, length);
    }
}

void ReplayingDecoderBuffer::getBytes(int index, std::string& dst, int dstIndex, int length) const {
    if (checkIndex(index, length)) {
        buffer->getBytes(index, dst, dstIndex, length);
    }
}

void ReplayingDecoderBuffer::getBytes(int index, ChannelBuffer& dst, int dstIndex, int length) const {
    if (checkIndex(index, length)) {
        buffer->getBytes(index, dst, dstIndex, length);
    }
}

void ReplayingDecoderBuffer::getBytes(int index, ChannelBuffer& dst, int length) const {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::getBytes(int index, ChannelBuffer& dst) const {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::getBytes(int index, OutputStream& out, int length) const {
    throw UnreplayableOperationException();
}

boost::int32_t ReplayingDecoderBuffer::getInt(int index) const {
    if (checkIndex(index, 4)) {
        return buffer->getInt(index);
    }
    return 0;
}

boost::uint32_t ReplayingDecoderBuffer::getUnsignedInt(int index) const {
    if (checkIndex(index, 4)) {
        return buffer->getUnsignedInt(index);
    }
    return 0;
}

boost::int64_t ReplayingDecoderBuffer::getLong(int index) const {
    if (checkIndex(index, 8)) {
        return buffer->getLong(index);
    }
    return 0;
}

boost::int32_t ReplayingDecoderBuffer::getMedium(int index) const {
    if (checkIndex(index, 3)) {
        return buffer->getMedium(index);
    }
    return 0;
}

boost::int32_t ReplayingDecoderBuffer::getUnsignedMedium(int index) const {
    if (checkIndex(index, 3)) {
        return buffer->getUnsignedMedium(index);
    }
    return 0;
}

boost::int16_t ReplayingDecoderBuffer::getShort(int index) const {
    if (checkIndex(index, 2)) {
        return buffer->getShort(index);
    }
    return 0;
}

boost::uint16_t ReplayingDecoderBuffer::getUnsignedShort(int index) const {
    if (checkIndex(index, 2)) {
        return buffer->getUnsignedShort(index);
    }
    return 0;
}

wchar_t ReplayingDecoderBuffer::getChar(int index) const {
    if (checkIndex(index, 2)) {
        return buffer->getChar(index);
    }
    return 0;
}

float ReplayingDecoderBuffer::getFloat(int index) const {
    if (checkIndex(index, 4)) {
        return buffer->getFloat(index);
    }
    return 0;
}

double ReplayingDecoderBuffer::getDouble(int index) const {
    if (checkIndex(index, 8)) {
        return buffer->getDouble(index);
    }
    return 0;
}

int ReplayingDecoderBuffer::hashCode() const {
    throw UnreplayableOperationException();
}

int ReplayingDecoderBuffer::indexOf(int fromIndex, int toIndex, boost::int8_t value) const {
    int endIndex = buffer->indexOf(fromIndex, toIndex, value);
    needMore = (endIndex < 0);

    return endIndex;
}

int ReplayingDecoderBuffer::indexOf(int fromIndex, int toIndex, const ChannelBufferIndexFinder& indexFinder) const {
    int endIndex = buffer->indexOf(fromIndex, toIndex, indexFinder);
    needMore = (endIndex < 0);

    return endIndex;
}

int ReplayingDecoderBuffer::bytesBefore(boost::int8_t value) const {
    int bytes = buffer->bytesBefore(value);
    needMore = (bytes < 0);
    return bytes;
}

int ReplayingDecoderBuffer::bytesBefore(const ChannelBufferIndexFinder& indexFinder) const {
    int bytes = buffer->bytesBefore(indexFinder);
    needMore = (bytes < 0);
    return bytes;
}

int ReplayingDecoderBuffer::bytesBefore(int length, boost::int8_t value) const {
    if (checkReadableBytes(length)) {
        int bytes = buffer->bytesBefore(length, value);
        needMore = (bytes < 0);
        return bytes;
    }

    return -1;
}

int ReplayingDecoderBuffer::bytesBefore(int length, const ChannelBufferIndexFinder& indexFinder) const {
    if (checkReadableBytes(length)) {
        int bytes = buffer->bytesBefore(length, indexFinder);
        needMore = (bytes < 0);
        return bytes;
    }

    return -1;
}

int ReplayingDecoderBuffer::bytesBefore(int index, int length, boost::int8_t value) const {
    int bytes = buffer->bytesBefore(index, length, value);
    needMore = (bytes < 0);
    return bytes;
}

int ReplayingDecoderBuffer::bytesBefore(int index, int length, const ChannelBufferIndexFinder& indexFinder) const {
    int bytes = buffer->bytesBefore(index, length, indexFinder);
    needMore = (bytes < 0);
    return bytes;
}

void ReplayingDecoderBuffer::markReaderIndex() {
    buffer->markReaderIndex();
}

void ReplayingDecoderBuffer::markWriterIndex() {
    throw UnreplayableOperationException();
}

ChannelBufferFactory& ReplayingDecoderBuffer::factory() const {
    return buffer->factory();
}

cetty::buffer::ByteOrder ReplayingDecoderBuffer::order() const {
    return buffer->order();
}

bool ReplayingDecoderBuffer::readable() const {
    return terminated ? buffer->readable() : true;
}

int ReplayingDecoderBuffer::readableBytes() const {
    if (terminated) {
        return buffer->readableBytes();
    }
    else {
        return Integer::MAX_VALUE - buffer->readerIndex();
    }
}

boost::int8_t ReplayingDecoderBuffer::readByte() {
    if (checkReadableBytes(1)) {
        return buffer->readByte();
    }
    return 0;
}

boost::uint8_t ReplayingDecoderBuffer::readUnsignedByte() {
    if (checkReadableBytes(1)) {
        return buffer->readUnsignedByte();
    }
    return 0;
}

void ReplayingDecoderBuffer::readBytes(const Array& dst, int dstIndex, int length) {
    if (checkReadableBytes(length)) {
        buffer->readBytes(dst, dstIndex, length);
    }
}

void ReplayingDecoderBuffer::readBytes(const Array& dst) {
    if (checkReadableBytes(dst.length())) {
        buffer->readBytes(dst);
    }
}

void ReplayingDecoderBuffer::readBytes(std::string& dst) {
    if (checkReadableBytes(readableBytes())) {
        buffer->readBytes(dst);
    }
}

void ReplayingDecoderBuffer::readBytes(std::string& dst, int length) {
    if (checkReadableBytes(length)) {
        buffer->readBytes(dst, length);
    }
}

void ReplayingDecoderBuffer::readBytes(std::string& dst, int dstIndex, int length) {
    if (checkReadableBytes(length)) {
        buffer->readBytes(dst, dstIndex, length);
    }
}

void ReplayingDecoderBuffer::readBytes(ChannelBuffer& dst, int dstIndex, int length) {
    if (checkReadableBytes(length)) {
        buffer->readBytes(dst, dstIndex, length);
    }
}

void ReplayingDecoderBuffer::readBytes(ChannelBuffer& dst, int length) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::readBytes(ChannelBuffer& dst) {
    throw UnreplayableOperationException();
}

cetty::buffer::ChannelBufferPtr ReplayingDecoderBuffer::readBytes() {
    return readBytes(readableBytes());
}

cetty::buffer::ChannelBufferPtr ReplayingDecoderBuffer::readBytes(int length) {
    if (checkReadableBytes(length)) {
        return buffer->readBytes(length);
    }
    return ChannelBuffers::EMPTY_BUFFER;
}

void ReplayingDecoderBuffer::readBytes(OutputStream& out, int length) {
    throw UnreplayableOperationException();
}

ChannelBufferPtr ReplayingDecoderBuffer::readSlice(int length) {
    if (checkReadableBytes(length)) {
        return buffer->readSlice(length);
    }
    return ChannelBuffers::EMPTY_BUFFER;
}

ChannelBufferPtr ReplayingDecoderBuffer::readSlice() {
    return readSlice(readableBytes());
}

int ReplayingDecoderBuffer::readerIndex() const {
    return buffer->readerIndex();
}

void ReplayingDecoderBuffer::readerIndex(int readerIndex) {
    buffer->readerIndex(readerIndex);
}

boost::int32_t ReplayingDecoderBuffer::readInt() {
    if (checkReadableBytes(4)) {
        return buffer->readInt();
    }
    return 0;
}

boost::uint32_t ReplayingDecoderBuffer::readUnsignedInt() {
    if (checkReadableBytes(4)) {
        return buffer->readUnsignedInt();
    }
    return 0;
}

boost::int64_t ReplayingDecoderBuffer::readLong() {
    if (checkReadableBytes(8)) {
        return buffer->readLong();
    }
    return 0;
}

boost::int32_t ReplayingDecoderBuffer::readMedium() {
    if (checkReadableBytes(3)) {
        return buffer->readMedium();
    }
    return 0;
}

boost::int32_t ReplayingDecoderBuffer::readUnsignedMedium() {
    if (checkReadableBytes(3)) {
        return buffer->readUnsignedMedium();
    }
    return 0;
}

boost::int16_t ReplayingDecoderBuffer::readShort() {
    if (checkReadableBytes(2)) {
        return buffer->readShort();
    }

    return 0;
}

boost::uint16_t ReplayingDecoderBuffer::readUnsignedShort() {
    if (checkReadableBytes(2)) {
        return buffer->readUnsignedShort();
    }
    return 0;
}

wchar_t ReplayingDecoderBuffer::readChar() {
    if (checkReadableBytes(2)) {
        return buffer->readChar();
    }
    return 0;
}

float ReplayingDecoderBuffer::readFloat() {
    if (checkReadableBytes(4)) {
        return buffer->readFloat();
    }
    return 0;
}

double ReplayingDecoderBuffer::readDouble() {
    if (checkReadableBytes(8)) {
        return buffer->readDouble();
    }
    return 0;
}

void ReplayingDecoderBuffer::resetReaderIndex() {
    buffer->resetReaderIndex();
}

void ReplayingDecoderBuffer::resetWriterIndex() {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setByte(int index, int value) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setBytes(int index, const ConstArray& src, int srcIndex, int length) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setBytes(int index, const ConstArray& src) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setBytes(int index, const std::string& src) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setBytes(int index, const std::string& src, int srcIndex, int length) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setBytes(int index, const ChannelBuffer& src, int srcIndex, int length) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setBytes(int index, ChannelBuffer& src, int length) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setBytes(int index, ChannelBuffer& src) {
    throw UnreplayableOperationException();
}

int ReplayingDecoderBuffer::setBytes(int index, InputStream& in, int length) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setZero(int index, int length) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setIndex(int readerIndex, int writerIndex) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setInt(int index, int value) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setLong(int index, boost::int64_t value) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setMedium(int index, int value) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setShort(int index, int value) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setChar(int index, int value) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setFloat(int index, float value) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::setDouble(int index, double value) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::skipBytes(int length) {
    if (checkReadableBytes(length)) {
        buffer->skipBytes(length);
    }
}

ChannelBufferPtr ReplayingDecoderBuffer::slice() {
    throw UnreplayableOperationException();
}

ChannelBufferPtr ReplayingDecoderBuffer::slice(int index, int length) {
    if (checkIndex(index, length)) {
        return buffer->slice(index, length);
    }

    return ChannelBuffers::EMPTY_BUFFER;
}

std::string ReplayingDecoderBuffer::toString() const {
    return std::string("ReplayingDecoderBuffer (ridx=") +
        Integer::toString(readerIndex()) +
        std::string(", widx=") +
        Integer::toString(writerIndex()) +
        std::string(")");
}

std::string ReplayingDecoderBuffer::toString(const Charset& charsetName) {
    throw UnreplayableOperationException();
}

std::string ReplayingDecoderBuffer::toString(int index, int length, const Charset& charset) {
    if (checkIndex(index, length)) {
        return buffer->toString(index, length, charset);
    }
    return "";
}

std::wstring ReplayingDecoderBuffer::toWideString(const Charset& charsetName) {
    throw UnreplayableOperationException();
}

std::wstring ReplayingDecoderBuffer::toWideString(int index, int length, const Charset& charset) {
    if (checkIndex(index, length)) {
        return buffer->toWideString(index, length, charset);
    }
    return L"";
}

bool ReplayingDecoderBuffer::writable() const {
    return false;
}

int ReplayingDecoderBuffer::writableBytes() const {
    return 0;
}

void ReplayingDecoderBuffer::writeByte(int value) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::writeBytes(const ConstArray& src, int srcIndex, int length) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::writeBytes(const ConstArray& src) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::writeBytes(const ChannelBuffer& src, int srcIndex, int length) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::writeBytes(ChannelBuffer& src, int length) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::writeBytes(ChannelBuffer& src) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::writeBytes(const std::string& src) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::writeBytes(const std::string& src, int srcIndex, int length) {
    throw UnreplayableOperationException();
}

int ReplayingDecoderBuffer::writeBytes(InputStream& in, int length) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::writeInt(int value) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::writeLong(boost::int64_t value) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::writeMedium(int value) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::writeZero(int length) {
    throw UnreplayableOperationException();
}

int ReplayingDecoderBuffer::writerIndex() const {
    return buffer->writerIndex();
}

void ReplayingDecoderBuffer::writerIndex(int writerIndex) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::writeShort(int value) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::writeChar(int value) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::writeFloat(float value) {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::writeDouble(double value) {
    throw UnreplayableOperationException();
}

}}}}