#if !defined(CETTY_HANDLER_CODEC_REPLAY_REPLAYINGDECODERBUFFER_H)
#define CETTY_HANDLER_CODEC_REPLAY_REPLAYINGDECODERBUFFER_H

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

namespace cetty { namespace handler { namespace codec { namespace replay {

using namespace cetty::buffer;

class ReplayingDecoderBuffer;

/**
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

typedef boost::intrusive_ptr<ReplayingDecoderBuffer> ReplayingDecoderBufferPtr;

class ReplayingDecoderBuffer : public cetty::buffer::ChannelBuffer {
public:
    ReplayingDecoderBuffer(const ChannelBufferPtr& buffer)
        : needMore(false), terminated(false), buffer(buffer) {
    }

    void terminate() {
        terminated = true;
    }

    bool needMoreBytes() const {
        return needMore;
    }

    virtual int capacity() const;

    virtual bool hasArray() const;

    virtual const Array& array();
    virtual ConstArray array() const;

    virtual int arrayOffset() const;

    virtual void clear();

    virtual bool equals(const ChannelBuffer& buffer) const;
    virtual int compareTo(const ChannelBuffer& buffer) const;

    virtual ChannelBufferPtr copy() const;
    virtual ChannelBufferPtr copy(int index, int length) const;

    virtual void discardReadBytes();

    virtual void ensureWritableBytes(int writableBytes);

    virtual ChannelBufferPtr duplicate();

    virtual boost::int8_t getByte(int index) const;
    virtual boost::uint8_t getUnsignedByte(int index) const;

    virtual void getBytes(int index, const Array& dst, int dstIndex, int length) const;
    virtual void getBytes(int index, const Array& dst) const;
    virtual void getBytes(int index, std::string& dst, int length) const;
    virtual void getBytes(int index, std::string& dst, int dstIndex, int length) const;
    virtual void getBytes(int index, ChannelBuffer& dst, int dstIndex, int length) const;
    virtual void getBytes(int index, ChannelBuffer& dst, int length) const;
    virtual void getBytes(int index, ChannelBuffer& dst) const;
    virtual void getBytes(int index, OutputStream& out, int length) const;

    virtual boost::int32_t  getInt(int index) const;
    virtual boost::uint32_t getUnsignedInt(int index) const;
    virtual boost::int64_t  getLong(int index) const;
    virtual boost::int32_t  getMedium(int index) const;
    virtual boost::int32_t  getUnsignedMedium(int index) const;
    virtual boost::int16_t  getShort(int index) const;
    virtual boost::uint16_t getUnsignedShort(int index) const;

    virtual wchar_t getChar(int index) const;
    virtual float getFloat(int index) const;
    virtual double getDouble(int index) const;

    virtual int hashCode() const;

    virtual int indexOf(int fromIndex, int toIndex, boost::int8_t value) const;
    virtual int indexOf(int fromIndex,
                        int toIndex,
                        const ChannelBufferIndexFinder& indexFinder) const;

    virtual int bytesBefore(boost::int8_t value) const;
    virtual int bytesBefore(const ChannelBufferIndexFinder& indexFinder) const;
    virtual int bytesBefore(int length, boost::int8_t value) const;
    virtual int bytesBefore(int length, const ChannelBufferIndexFinder& indexFinder) const;
    virtual int bytesBefore(int index, int length, boost::int8_t value) const;
    virtual int bytesBefore(int index,
                            int length,
                            const ChannelBufferIndexFinder& indexFinder) const;

    virtual void markReaderIndex();
    virtual void markWriterIndex();

    virtual ChannelBufferFactory& factory() const;
    virtual ByteOrder order() const;

    virtual bool readable() const;
    virtual int  readableBytes() const;
    virtual void readableBytes(Array& array) {}

    virtual boost::int8_t readByte();
    virtual boost::uint8_t readUnsignedByte();

    virtual void readBytes(const Array& dst, int dstIndex, int length);
    virtual void readBytes(const Array& dst);
    virtual void readBytes(std::string& dst);
    virtual void readBytes(std::string& dst, int length);
    virtual void readBytes(std::string& dst, int dstIndex, int length);
    virtual void readBytes(ChannelBuffer& dst, int dstIndex, int length);
    virtual void readBytes(ChannelBuffer& dst, int length);
    virtual void readBytes(ChannelBuffer& dst);

    virtual ChannelBufferPtr readBytes();
    virtual ChannelBufferPtr readBytes(int length);
    virtual ChannelBufferPtr readSlice();
    virtual ChannelBufferPtr readSlice(int length);

    virtual void readSlice(Array& array){}
    virtual void readSlice(GatheringBuffer& gathering) {}

    virtual void readBytes(OutputStream& out, int length);

    virtual int  readerIndex() const;
    virtual void readerIndex(int readerIndex);
    virtual void offsetReaderIndex(int offset) {}

    virtual boost::int32_t  readInt();
    virtual boost::uint32_t readUnsignedInt();
    virtual boost::int64_t  readLong();
    virtual boost::int32_t  readMedium();
    virtual boost::int32_t  readUnsignedMedium();
    virtual boost::int16_t  readShort();
    virtual boost::uint16_t readUnsignedShort();

    virtual wchar_t readChar();
    virtual float   readFloat();
    virtual double  readDouble();

    virtual void resetReaderIndex();
    virtual void resetWriterIndex();

    virtual void setByte (int index, int value);
    virtual void setBytes(int index, const ConstArray& src, int srcIndex, int length);
    virtual void setBytes(int index, const ConstArray& src);
    virtual void setBytes(int index, const std::string& src);
    virtual void setBytes(int index, const std::string& src, int srcIndex, int length);
    virtual void setBytes(int index, const ChannelBuffer& src, int srcIndex, int length);
    virtual void setBytes(int index, ChannelBuffer& src, int length);
    virtual void setBytes(int index, ChannelBuffer& src);
    virtual int  setBytes(int index, InputStream& in, int length);

    virtual void setZero(int index, int length);
    virtual void setIndex(int readerIndex, int writerIndex);
    virtual void setInt(int index, int value);
    virtual void setLong(int index, boost::int64_t value);
    virtual void setMedium(int index, int value);
    virtual void setShort(int index, int value);
    virtual void setChar(int index, int value);
    virtual void setFloat(int index, float value);
    virtual void setDouble(int index, double value);

    virtual void skipBytes(int length);

    virtual ChannelBufferPtr slice();
    virtual ChannelBufferPtr slice(int index, int length);
    virtual void slice(Array& array) {}

    virtual std::string toString(const Charset& charsetName);
    virtual std::string toString(int index, int length, const Charset& charset);

    virtual std::wstring toWideString(const Charset& charsetName);
    virtual std::wstring toWideString(int index, int length, const Charset& charset);

    virtual std::string toString() const;

    virtual bool writable() const;
    virtual int  writableBytes() const;
    virtual void writableBytes(Array& array) {}

    virtual void writeByte(int value);
    virtual void writeBytes(const ConstArray& src, int srcIndex, int length);
    virtual void writeBytes(const ConstArray& src);
    virtual void writeBytes(const ChannelBuffer& src, int srcIndex, int length);
    virtual void writeBytes(ChannelBuffer& src, int length);
    virtual void writeBytes(ChannelBuffer& src);
    virtual void writeBytes(const std::string& src);
    virtual void writeBytes(const std::string& src, int srcIndex, int length);
    virtual int  writeBytes(InputStream& in, int length);
    virtual void writeInt(int value);
    virtual void writeLong(boost::int64_t value);
    virtual void writeMedium(int value);
    virtual void writeZero(int length);

    virtual int  writerIndex() const;
    virtual void writerIndex(int writerIndex);
    virtual void offsetWriterIndex(int offset) {}

    virtual void writeShort(int value);
    virtual void writeChar(int value);
    virtual void writeFloat(float value);
    virtual void writeDouble(double value);

private:
    inline bool checkIndex(int index) const {
        if (index <= buffer->writerIndex()) {
            needMore = false;
            return true;
        }

        needMore = true;
        return false;
    }

    inline bool checkIndex(int index, int length) const {
        if (index + length <= buffer->writerIndex()) {
            needMore = false;
            return true;
        }

        needMore = true;
        return false;
    }

    inline bool checkReadableBytes(int readableBytes) const {
        if (buffer->readableBytes() >= readableBytes) {
            needMore = false;
            return true;
        }

        needMore = true;
        return false;
    }

private:
    mutable bool needMore;
    bool         terminated;
    ChannelBufferPtr buffer;
};


}}}}

#endif //#if !defined(CETTY_HANDLER_CODEC_REPLAY_REPLAYINGDECODERBUFFER_H)
