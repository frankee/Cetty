#if !defined(CETTY_BUFFER_ABSTRACTCHANNELBUFFER_H)
#define CETTY_BUFFER_ABSTRACTCHANNELBUFFER_H

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

namespace cetty { namespace buffer {

/**
 * A skeletal implementation of a buffer.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2211 $, $Date: 2010-03-04 15:34:00 +0900 (Thu, 04 Mar 2010) $
 */

class AbstractChannelBuffer : public ChannelBuffer {
public:
    AbstractChannelBuffer()
        : readerIdx(0), writerIdx(0), markedReaderIndex(0), markedWriterIndex(0) {}
    
    AbstractChannelBuffer(int readerIdx, int writerIdx)
        : readerIdx(readerIdx), writerIdx(writerIdx), markedReaderIndex(0), markedWriterIndex(0) {}

    virtual ~AbstractChannelBuffer() {}

    virtual int  readerIndex() const;
    virtual void readerIndex(int readerIdx);
    virtual void offsetReaderIndex(int offset);

    virtual int  writerIndex() const;
    virtual void writerIndex(int writerIdx);
    virtual void offsetWriterIndex(int offset);

    virtual void setIndex(int readerIndex, int writerIndex);

    virtual void clear();

    virtual bool readable() const;
    virtual bool writable() const;

    virtual int readableBytes() const;
    virtual int writableBytes() const;

    virtual void markReaderIndex();
    virtual void resetReaderIndex();
    virtual void markWriterIndex();
    virtual void resetWriterIndex();
    virtual void discardReadBytes();

    virtual void ensureWritableBytes(int wBytes);

    virtual boost::uint8_t getUnsignedByte(int index) const;
    virtual boost::uint16_t getUnsignedShort(int index) const;
    virtual boost::int32_t getMedium(int index) const;
    virtual boost::uint32_t getUnsignedInt(int index) const;
    virtual wchar_t getChar(int index) const;

    virtual float getFloat(int index) const;
    virtual double getDouble(int index) const;

    virtual void getBytes(int index, const Array& dst) const;
    virtual void getBytes(int index, const Array& dst, int dstIndex, int length) const = 0;
    virtual void getBytes(int index, std::string& dst, int length) const;
    virtual void getBytes(int index, std::string& dst, int dstIndex, int length) const;
    virtual void getBytes(int index, ChannelBuffer& dst) const;
    virtual void getBytes(int index, ChannelBuffer& dst, int length) const;
    virtual void getBytes(int index, ChannelBuffer& dst, int dstIndex, int length) const = 0;
    virtual void getBytes(int index, OutputStream& out, int length) const = 0;
    virtual void setChar(int index, int value);
    virtual void setFloat(int index, float value);
    virtual void setDouble(int index, double value);

    virtual void setBytes(int index, const ConstArray& src);
    virtual void setBytes(int index, const ConstArray& src, int srcIndex, int length) = 0;
    virtual void setBytes(int index, const std::string& src);
    virtual void setBytes(int index, const std::string& src, int srcIndex, int length);
    virtual void setBytes(int index, ChannelBuffer& src);
    virtual void setBytes(int index, ChannelBuffer& src, int length);
    virtual void setBytes(int index, const ChannelBuffer& src, int srcIndex, int length) = 0;
    virtual int  setBytes(int index, InputStream& in, int length) = 0;

    virtual void setZero(int index, int length);

    virtual boost::int8_t readByte();
    virtual boost::uint8_t readUnsignedByte();
    virtual boost::int16_t readShort();
    virtual boost::uint16_t readUnsignedShort();
    virtual boost::int32_t readMedium();
    virtual boost::int32_t readUnsignedMedium();
    virtual boost::int32_t readInt();
    virtual boost::uint32_t readUnsignedInt();
    virtual boost::int64_t readLong();
    virtual wchar_t readChar();
    virtual float readFloat();
    virtual double readDouble();

    virtual ChannelBufferPtr readBytes();
    virtual ChannelBufferPtr readBytes(int length);

    virtual void readSlice(Array& array) = 0;

    virtual ChannelBufferPtr readSlice();
    virtual ChannelBufferPtr readSlice(int length);

    virtual void readBytes(const Array& dst, int dstIndex, int length);
    virtual void readBytes(const Array& dst);
    virtual void readBytes(std::string& dst);
    virtual void readBytes(std::string& dst, int length);
    virtual void readBytes(std::string& dst, int dstIndex, int length);
    virtual void readBytes(ChannelBuffer& dst);
    virtual void readBytes(ChannelBuffer& dst, int length);
    virtual void readBytes(ChannelBuffer& dst, int dstIndex, int length);
    virtual void readBytes(OutputStream& out, int length);

    virtual void skipBytes(int length);

    virtual void writeByte(int value);
    virtual void writeShort(int value);
    virtual void writeMedium(int value);
    virtual void writeInt(int value);
    virtual void writeLong(boost::int64_t value);
    virtual void writeChar(int value);
    virtual void writeFloat(float value);
    virtual void writeDouble(double value);

    virtual void writeBytes(const ConstArray& src, int srcIndex, int length);
    virtual void writeBytes(const ConstArray& src);
    virtual void writeBytes(const std::string& src);
    virtual void writeBytes(const std::string& src, int srcIndex, int length);
    virtual void writeBytes(ChannelBuffer& src);
    virtual void writeBytes(ChannelBuffer& src, int length);
    virtual void writeBytes(const ChannelBuffer& src, int srcIndex, int length);
    virtual int  writeBytes(InputStream& in, int length);

    virtual void writeZero(int length);

    virtual ChannelBufferPtr copy() const;
    virtual ChannelBufferPtr copy(int index, int length) const = 0;

    virtual ChannelBufferPtr slice();
    virtual ChannelBufferPtr slice(int index, int length) = 0;

    virtual std::string toString(const Charset& charset);
    virtual std::string toString(int index, int length, const Charset& charset);
    virtual std::wstring toWideString(const Charset& charset);
    virtual std::wstring toWideString(int index, int length, const Charset& charset);

    virtual int indexOf(int fromIndex, int toIndex, boost::int8_t value) const;
    virtual int indexOf(int fromIndex,
                        int toIndex,
                        const ChannelBufferIndexFinder& indexFinder) const;

    virtual int bytesBefore(boost::int8_t value) const;
    virtual int bytesBefore(const ChannelBufferIndexFinder& indexFinder) const;
    virtual int bytesBefore(int length, boost::int8_t value) const;
    virtual int bytesBefore(int length,
                            const ChannelBufferIndexFinder& indexFinder) const;
    virtual int bytesBefore(int index, int length, boost::int8_t value) const;
    virtual int bytesBefore(int index,
                            int length,
                            const ChannelBufferIndexFinder& indexFinder) const;

    virtual int hashCode() const;

    virtual bool equals(const ChannelBuffer& cb) const;
    virtual int  compareTo(const ChannelBuffer& that) const;

    virtual std::string toString() const;

protected:
    /**
     * Throws an {@link RangeException} if the current
     * {@link #readableBytes() readable bytes} of this buffer is less
     * than the specified value.
     */
    virtual void checkReadableBytes(int minimumReadableBytes) const;

protected:
	int readerIdx;
    int writerIdx;
    int markedReaderIndex;
    int markedWriterIndex;
};

}}


#endif //#if !defined(CETTY_BUFFER_ABSTRACTCHANNELBUFFER_H)
