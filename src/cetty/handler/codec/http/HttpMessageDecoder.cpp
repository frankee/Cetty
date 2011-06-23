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
#include "cetty/handler/codec/http/HttpMessageDecoder.h"

#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/util/Integer.h"
#include "cetty/util/Character.h"
#include "cetty/util/Exception.h"

#include "cetty/handler/codec/http/HttpVersion.h"
#include "cetty/handler/codec/http/HttpHeader.h"
#include "cetty/handler/codec/http/HttpHeaders.h"
#include "cetty/handler/codec/http/HttpMessage.h"
#include "cetty/handler/codec/http/HttpResponse.h"
#include "cetty/handler/codec/http/HttpResponseStatus.h"
#include "cetty/handler/codec/http/HttpCodecUtil.h"
#include "cetty/handler/codec/http/DefaultHttpChunk.h"
#include "cetty/handler/codec/http/DefaultHttpChunkTrailer.h"
#include "cetty/handler/codec/frame/TooLongFrameException.h"


namespace cetty { namespace handler { namespace codec { namespace http {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;
using namespace cetty::handler::codec::frame;

HttpMessageDecoder::FastString
HttpMessageDecoder::FastString::substring(int start, int end) const {
    if (start < end) {
        return FastString(data + start, end - start);
    }
    return FastString();
}

HttpMessageDecoder::FastString HttpMessageDecoder::FastString::trim() const {
    char* newData = data;
    int   newSize = size;
    while((*newData == ' ' || *newData == '\t') && newSize > 0) {
        newData++;
        --newData;
    }

    while((newData[newSize - 1] == ' ' || newData[newSize - 1] == '\t')
        && newSize > 0) {
            --newSize;
    }

    newData[newSize] = '\0';
    return FastString(newData, newSize);
}

bool HttpMessageDecoder::FastString::equalsIgnoreCase(const std::string& str) const {
    return true;
}

HttpMessageDecoder::HttpMessageDecoder()
    : ReplayingDecoder(SKIP_CONTROL_CHARS, true),
      maxInitialLineLength(4096),
      maxHeaderSize(8192),
      maxChunkSize(8192),
      chunkSize(0),
      headerSize(0) {
}

HttpMessageDecoder::HttpMessageDecoder(int maxInitialLineLength,
                                       int maxHeaderSize,
                                       int maxChunkSize)
    : ReplayingDecoder(SKIP_CONTROL_CHARS, true),
      maxInitialLineLength(maxInitialLineLength),
      maxHeaderSize(maxHeaderSize),
      maxChunkSize(maxChunkSize),
      chunkSize(0),
      headerSize(0) {
    if (maxInitialLineLength <= 0) {
        throw InvalidArgumentException(
            std::string("maxInitialLineLength must be a positive integer: ") +
            Integer::toString(maxInitialLineLength));
    }
    if (maxHeaderSize <= 0) {
        throw InvalidArgumentException(
            std::string("maxHeaderSize must be a positive integer: ") +
            Integer::toString(maxHeaderSize));
    }
    if (maxChunkSize < 0) {
        throw InvalidArgumentException(
            std::string("maxChunkSize must be a positive integer: ") +
            Integer::toString(maxChunkSize));
    }
}

ChannelMessage HttpMessageDecoder::decode(ChannelHandlerContext& ctx,
                                          Channel& channel,
                                          const ReplayingDecoderBufferPtr& buffer,
                                          int state) {
    switch (state) {
        case SKIP_CONTROL_CHARS: {
            if (skipControlCharacters(buffer)) {
                checkpoint(READ_INITIAL);
            }
            else {
                checkpoint(); //do not care skipped control chars.
            }
            break;
        }
        case READ_INITIAL: {
            std::vector<FastString> initialLine;
            FastString line = readLine(buffer, maxInitialLineLength);
            if (!splitInitialLine(line, initialLine)) {
                    // need more bytes.
                    break;
            }

            if (initialLine.size() < 3) {
                // Invalid initial line - ignore.
                checkpoint(SKIP_CONTROL_CHARS);
                break;
            }

            message = createMessage(initialLine[0], initialLine[1], initialLine[2]);
            checkpoint(READ_HEADER);

            // clear data, then step into the READ_HEADER state.
            headerSize = 0;
            message->clearHeaders();
            break;
        }
        case READ_HEADER: {
            int nextState = readHeaders(buffer);
            if (nextState == READ_HEADER) {
                // need read more bytes to parse headers.
                break;
            }

            checkpoint(nextState);
            if (nextState == READ_CHUNK_SIZE) {
                // Chunked encoding
                message->setChunked(true);

                // Generate HttpMessage first.  HttpChunks will follow.
                return ChannelMessage(message);
            }
            else if (nextState == SKIP_CONTROL_CHARS) {
                // No content is expected.
                // Remove the headers which are not supposed to be present not
                // to confuse subsequent handlers.
                message->header().remove(HttpHeaders::Names::TRANSFER_ENCODING);
                return ChannelMessage(message);
            }
            else {
                int contentLength = HttpHeaders::getContentLength(*message, -1);
                if (contentLength == 0 || (contentLength == -1 && isDecodingRequest())) {
                    content = ChannelBuffers::EMPTY_BUFFER;
                    return reset();
                }

                switch (nextState) {
                case READ_FIXED_LENGTH_CONTENT:
                    if (contentLength > maxChunkSize
                        || HttpHeaders::is100ContinueExpected(*message)) {
                            // Generate HttpMessage first.  HttpChunks will follow.
                            checkpoint(READ_FIXED_LENGTH_CONTENT_AS_CHUNKS);
                            message->setChunked(true);
                            // chunkSize will be decreased as the READ_FIXED_LENGTH_CONTENT_AS_CHUNKS
                            // state reads data chunk by chunk.
                            chunkSize = HttpHeaders::getContentLength(*message, -1);
                            return ChannelMessage(message);
                    }
                    break;
                case READ_VARIABLE_LENGTH_CONTENT:
                    if (buffer->readableBytes() > maxChunkSize
                        || HttpHeaders::is100ContinueExpected(*message)) {
                            // Generate HttpMessage first.  HttpChunks will follow.
                            checkpoint(READ_VARIABLE_LENGTH_CONTENT_AS_CHUNKS);
                            message->setChunked(true);
                            return ChannelMessage(message);
                    }
                    break;
                default:
                    throw IllegalStateException(
                        std::string("Unexpected state: ") +
                        Integer::toString(nextState));
                }
            }

            // We return null here, this forces decode to be called again where we will decode the content
            return ChannelMessage::EMPTY_MESSAGE;
        }
        case READ_VARIABLE_LENGTH_CONTENT: {
            content = buffer->readSlice(buffer->readableBytes());
            if (buffer->needMoreBytes()) {
                return ChannelMessage::EMPTY_MESSAGE;
            }
            return reset();
        }
        case READ_VARIABLE_LENGTH_CONTENT_AS_CHUNKS: {
            // Keep reading data as a chunk until the end of connection is reached.
            int chunkSize = std::min(maxChunkSize, buffer->readableBytes());
            ChannelBufferPtr buff = buffer->readSlice(chunkSize);
            if (buffer->needMoreBytes()) {
                return ChannelMessage::EMPTY_MESSAGE;
            }

            HttpChunkPtr chunk = HttpChunkPtr(new DefaultHttpChunk(buff));
            if (!buffer->readable()) {
                // Reached to the end of the connection.
                reset();
                if (!chunk->isLast()) {
                    // Append the last chunk.
                    return ChannelMessage(ChannelMessage(chunk),
                                          ChannelMessage(HttpChunk::LAST_CHUNK));
                }
            }
            return ChannelMessage(chunk);
        }
        case READ_FIXED_LENGTH_CONTENT: {
            //we have a content-length so we just read the correct number of bytes
            if (readFixedLengthContent(buffer)) {
                return reset();
            }

            return ChannelMessage::EMPTY_MESSAGE;
        }
        case READ_FIXED_LENGTH_CONTENT_AS_CHUNKS: {
            int chunkSize = this->chunkSize;
            HttpChunkPtr chunk;
            ChannelBufferPtr buff;
            if (chunkSize > maxChunkSize) {
                buff = buffer->readSlice(maxChunkSize);
                if (buffer->needMoreBytes()) {
                    return ChannelMessage::EMPTY_MESSAGE;
                }
                chunkSize -= maxChunkSize;
            }
            else {
                BOOST_ASSERT(chunkSize <= Integer::MAX_VALUE);
                buff = buffer->readSlice(chunkSize);
                if (buffer->needMoreBytes()) {
                    return ChannelMessage::EMPTY_MESSAGE;
                }
                chunkSize = 0;
            }
            chunk = HttpChunkPtr(new DefaultHttpChunk(buff));
            this->chunkSize = chunkSize;

            if (chunkSize == 0) {
                // Read all content.
                reset();
                if (!chunk->isLast()) {
                    // Append the last chunk.
                    return ChannelMessage(ChannelMessage(chunk),
                                          ChannelMessage(HttpChunk::LAST_CHUNK));
                }
            }
            return ChannelMessage(chunk);
        }
       /**
        * everything else after this point takes care of reading chunked content. basically, read chunk size,
        * read chunk, read and ignore the CRLF and repeat until 0
        */
        case READ_CHUNK_SIZE: {
            FastString line = readLine(buffer, maxInitialLineLength);
            if (buffer->needMoreBytes()) {
                return ChannelMessage::EMPTY_MESSAGE;
            }

            chunkSize = getChunkSize(line);
            if (chunkSize == 0) {
                checkpoint(READ_CHUNK_FOOTER);
                return ChannelMessage::EMPTY_MESSAGE;
            }
            else if (chunkSize > maxChunkSize) {
                // A chunk is too large. Split them into multiple chunks again.
                checkpoint(READ_CHUNKED_CONTENT_AS_CHUNKS);
            }
            else {
                checkpoint(READ_CHUNKED_CONTENT);
            }
            break;
        }
        case READ_CHUNKED_CONTENT: {
            BOOST_ASSERT(chunkSize <= Integer::MAX_VALUE);
            ChannelBufferPtr buff = buffer->readSlice(chunkSize);
            if (buffer->needMoreBytes()) {
                return ChannelMessage::EMPTY_MESSAGE;
            }

            HttpChunkPtr chunk = HttpChunkPtr(new DefaultHttpChunk(buff));
            checkpoint(READ_CHUNK_DELIMITER);
            return ChannelMessage(chunk);
        }
        case READ_CHUNKED_CONTENT_AS_CHUNKS: {
            long chunkSize = this->chunkSize;
            HttpChunkPtr chunk;
            ChannelBufferPtr buff;
            if (chunkSize > maxChunkSize) {
                buff = buffer->readSlice(maxChunkSize);
                if (buffer->needMoreBytes()) {
                    return ChannelMessage::EMPTY_MESSAGE;
                }
                chunkSize -= maxChunkSize;
            }
            else {
                BOOST_ASSERT(chunkSize <= Integer::MAX_VALUE);
                buff = buffer->readSlice(chunkSize);
                if (buffer->needMoreBytes()) {
                    return ChannelMessage::EMPTY_MESSAGE;
                }
                chunkSize = 0;
            }
            chunk = HttpChunkPtr(new DefaultHttpChunk(buff));
            this->chunkSize = chunkSize;

            if (chunkSize == 0) {
                // Read all content.
                checkpoint(READ_CHUNK_DELIMITER);
            }

            if (!chunk->isLast()) {
                return ChannelMessage(chunk);
            }
            break;
        }
        case READ_CHUNK_DELIMITER: {
            for (;;) {
                boost::int8_t next = buffer->readByte();
                if (buffer->needMoreBytes()) {
                    return ChannelMessage::EMPTY_MESSAGE;
                }

                if (next == HttpCodecUtil::CR) {
                    if (buffer->readByte() == HttpCodecUtil::LF) {
                        checkpoint(READ_CHUNK_SIZE);
                        return ChannelMessage::EMPTY_MESSAGE;
                    }

                    if (buffer->needMoreBytes()) {
                        return ChannelMessage::EMPTY_MESSAGE;
                    }
                }
                else if (next == HttpCodecUtil::LF) {
                    checkpoint(READ_CHUNK_SIZE);
                    return ChannelMessage::EMPTY_MESSAGE;
                }
            }
            break;
        }
        case READ_CHUNK_FOOTER: {
            HttpChunkTrailerPtr trailer = readTrailingHeaders(buffer);
            if (maxChunkSize == 0) {
                // Chunked encoding disabled.
                return reset();
            }
            else {
                reset();
                // The last chunk, which is empty
                return ChannelMessage(trailer);
            }
        }
        default: {
            throw RuntimeException("Shouldn't reach here.");
        }
    }

    return ChannelMessage::EMPTY_MESSAGE;
}

bool HttpMessageDecoder::isContentAlwaysEmpty(const HttpMessage& msg) const {
    const HttpResponse* response = dynamic_cast<const HttpResponse*>(&msg);
    if (response) {
        int code = response->getStatus().getCode();
        if (code < 200) {
            return true;
        }
        switch (code) {
            case 204: case 205: case 304:
                return true;
        }
    }
    return false;
}

cetty::channel::ChannelMessage HttpMessageDecoder::reset() {
    HttpMessagePtr message = this->message;
    ChannelBufferPtr content = this->content;

    if (content) {
        message->setContent(content);
        this->content.reset();
    }
    this->message.reset();

    checkpoint(SKIP_CONTROL_CHARS);
    return ChannelMessage(message);
}

bool HttpMessageDecoder::skipControlCharacters(const ReplayingDecoderBufferPtr& buffer) const {
    for (;;) {
        boost::uint8_t c = buffer->readUnsignedByte();
        if (buffer->needMoreBytes()) return false;

        if (!Character::isISOControl(c) &&
            !Character::isWhitespace(c)) {
                buffer->readerIndex(buffer->readerIndex() - 1);
                return true;
        }
    }
}

bool HttpMessageDecoder::readFixedLengthContent(const ReplayingDecoderBufferPtr& buffer) {
    int length = HttpHeaders::getContentLength(*message, -1);
    BOOST_ASSERT(length <= Integer::MAX_VALUE);

    content.reset();
    content = buffer->readSlice(length);

    return !(buffer->needMoreBytes());
}

int HttpMessageDecoder::readHeaders(const ReplayingDecoderBufferPtr& buffer) {
    int nextState;
    FastString line = readHeader(buffer);
    FastString name;
    FastString value;

    headerSize = 0;
    if (buffer->needMoreBytes()) { // need more data
        return READ_HEADER;
    }

    if (line.length() != 0) {
        std::vector<FastString> header;
        std::string multiValueStr;

        while (line.length() != 0) {
            char firstChar = line.at(0);
            if (!name.empty() && (firstChar == ' ' || firstChar == '\t')) {
                multiValueStr.clear();
                FastString trimedLine = line.trim();

                multiValueStr.append(value.c_str());
                multiValueStr.append(1, ' ');
                multiValueStr.append(trimedLine.data, trimedLine.size);
            }
            else {
                if (!name.empty()) {
                    if (!multiValueStr.empty()) {
                        message->addHeader(name.c_str(), multiValueStr);
                    }
                    else {
                        message->addHeader(name.c_str(), value.c_str());
                    }
                }

                header.clear();
                splitHeader(line, header);
                name = header[0];
                value = header[1];
            }

            line = readHeader(buffer);
            if (buffer->needMoreBytes()) { // need more data
                return READ_HEADER;
            }
        }

        // Add the last header.
        if (!name.empty()) {
            if (!multiValueStr.empty()) {
                message->addHeader(name.c_str(), multiValueStr);
            }
            else {
                message->addHeader(name.c_str(), value.c_str());
            }
        }
    }

    if (isContentAlwaysEmpty(*message)) {
        nextState = SKIP_CONTROL_CHARS;
    }
    else if (message->isChunked()) {
        // HttpMessage.isChunked() returns true when either:
        // 1) HttpMessage.setChunked(true) was called or
        // 2) 'Transfer-Encoding' is 'chunked'.
        // Because this decoder did not call HttpMessage.setChunked(true)
        // yet, HttpMessage.isChunked() should return true only when
        // 'Transfer-Encoding' is 'chunked'.
        nextState = READ_CHUNK_SIZE;
    }
    else if (HttpHeaders::getContentLength(*message, -1) >= 0) {
        nextState = READ_FIXED_LENGTH_CONTENT;
    }
    else {
        nextState = READ_VARIABLE_LENGTH_CONTENT;
    }
    return nextState;
}

HttpChunkTrailerPtr
HttpMessageDecoder::readTrailingHeaders(const ReplayingDecoderBufferPtr& buffer) {
    headerSize = 0;
    FastString line = readHeader(buffer);
    FastString lastHeader;
    if (line.length() != 0) {
        std::vector<FastString> header;
        HttpChunkTrailerPtr trailer = HttpChunkTrailerPtr(new DefaultHttpChunkTrailer);
        do {
            char firstChar = line.at(0);
            if (!lastHeader.empty() && (firstChar == ' ' || firstChar == '\t')) {
                HttpHeader::StringList current = trailer->header().gets(lastHeader.c_str());
                if (current.size() != 0) {
                    current.back().append(line.trim().c_str());
                }
                else {
                    // Content-Length, Transfer-Encoding, or Trailer
                }
            }
            else {
                header.clear();
                splitHeader(line, header);
                FastString name = header[0];
                if (!name.equalsIgnoreCase(HttpHeaders::Names::CONTENT_LENGTH) &&
                    !name.equalsIgnoreCase(HttpHeaders::Names::TRANSFER_ENCODING) &&
                    !name.equalsIgnoreCase(HttpHeaders::Names::TRAILER)) {
                        trailer->header().add(name.c_str(), header[1].c_str());
                }
                lastHeader = name;
            }

            line = readHeader(buffer);
        }
        while (line.length() != 0);

        return trailer;
    }

    return boost::dynamic_pointer_cast<HttpChunkTrailer>(HttpChunk::LAST_CHUNK);
}

HttpMessageDecoder::FastString
HttpMessageDecoder::readHeader(const ReplayingDecoderBufferPtr& buffer) {
    FastString str;
    str.data = buffer->array().data(buffer->readerIndex());

    int headerSize = this->headerSize;
    int strSize = 0;
    while (true) {
        boost::int8_t nextByte = buffer->readByte();
        if (buffer->needMoreBytes()) return str;

        ++headerSize;
        ++strSize;
        if (nextByte == HttpCodecUtil::CR) {
            nextByte = buffer->readByte();
            if (buffer->needMoreBytes()) return str;

            ++headerSize;
            if (nextByte == HttpCodecUtil::LF) {
                break;
            }
        }
        else if (nextByte == HttpCodecUtil::LF) {
            break;
        }

        // Abort decoding if the header part is too large.
        if (headerSize >= maxHeaderSize) {
            // TODO: Respond with Bad Request and discard the traffic
            //    or close the connection.
            //       No need to notify the upstream handlers - just log.
            //       If decoding a response, just throw an exception.
            throw TooLongFrameException(
                std::string("HTTP header is larger than ") +
                Integer::toString(maxHeaderSize) +
                std::string(" bytes."));

        }
    }
    str.data[strSize - 1] = '\0';
    str.size = strSize - 1;

    this->headerSize = headerSize;
    return str;
}

int HttpMessageDecoder::getChunkSize(const FastString& hex) const {
    FastString h = hex.trim();
    for (int i = 0; i < h.length(); ++i) {
        char c = h.at(i);
        if (c == ';' || Character::isWhitespace(c) || Character::isISOControl(c)) {
            h = h.substring(0, i);
            break;
        }
    }

    return Integer::parse(h.c_str(), 16);
}

HttpMessageDecoder::FastString HttpMessageDecoder::readLine(const ReplayingDecoderBufferPtr& buffer, int maxLineLength) {
    FastString str;
    str.data = buffer->array().data(buffer->readerIndex());

    while (true) {
        boost::int8_t nextByte = buffer->readByte();
        if (buffer->needMoreBytes()) return str;

        if (nextByte == HttpCodecUtil::CR) {
            nextByte = buffer->readByte();
            if (buffer->needMoreBytes()) return str;

            if (nextByte == HttpCodecUtil::LF) {
                break;
            }
        }
        else if (nextByte == HttpCodecUtil::LF) {
            break;
        }
        else {
            if (str.size >= maxLineLength) {
                // TODO: Respond with Bad Request and discard the traffic
                //    or close the connection.
                //       No need to notify the upstream handlers - just log.
                //       If decoding a response, just throw an exception.
                throw TooLongFrameException(
                    std::string("An HTTP line is larger than ") +
                    Integer::toString(maxLineLength) +
                    std::string(" bytes."));
            }
            ++str.size;
        }
    }

    // make sure sb.data is exactly a c string.
    str[str.size] = '\0';
    --str.size;

    return str;
}

bool HttpMessageDecoder::splitInitialLine(FastString& sb, std::vector<FastString>& lines) {
    int aStart;
    int aEnd;
    int bStart;
    int bEnd;
    int cStart;
    int cEnd;

    if (sb.empty()) return false;

    aStart = findNonWhitespace(sb, 0);
    aEnd = findWhitespace(sb, aStart);

    bStart = findNonWhitespace(sb, aEnd);
    bEnd = findWhitespace(sb, bStart);

    cStart = findNonWhitespace(sb, bEnd);
    cEnd = findEndOfString(sb);

    sb[aEnd] = '\0';
    sb[bEnd] = '\0';
    lines.push_back(sb.substring(aStart, aEnd));
    lines.push_back(sb.substring(bStart, bEnd));
    lines.push_back(sb.substring(cStart, cEnd));

    return true;
}

void HttpMessageDecoder::splitHeader(FastString& sb, std::vector<FastString>& header) {
    int length = sb.length();
    int nameStart;
    int nameEnd;
    int colonEnd;
    int valueStart;
    int valueEnd;

    nameStart = findNonWhitespace(sb, 0);
    for (nameEnd = nameStart; nameEnd < length; ++nameEnd) {
        char ch = sb.at(nameEnd);
        if (ch == ':' || Character::isWhitespace(ch)) {
            break;
        }
    }

    for (colonEnd = nameEnd; colonEnd < length; ++colonEnd) {
        if (sb.at(colonEnd) == ':') {
            ++colonEnd;
            break;
        }
    }

    valueStart = findNonWhitespace(sb, colonEnd);
    if (valueStart == length) {
        header.push_back(sb.substring(nameStart, nameEnd));
        header.push_back(FastString());
        return;
    }

    valueEnd = findEndOfString(sb);

    sb[nameEnd] = '\0';
    header.push_back(sb.substring(nameStart, nameEnd));
    header.push_back(sb.substring(valueStart, valueEnd));
}

int HttpMessageDecoder::findNonWhitespace(const FastString& sb, int offset) {
    int result;
    for (result = offset; result < sb.length(); ++result) {
        if (!Character::isWhitespace(sb.at(result))) {
            break;
        }
    }
    return result;
}

int HttpMessageDecoder::findWhitespace(const FastString& sb, int offset) {
    int result;
    for (result = offset; result < sb.length(); ++result) {
        if (Character::isWhitespace(sb.at(result))) {
            break;
        }
    }
    return result;
}

int HttpMessageDecoder::findEndOfString(const FastString& sb) {
    int result;
    for (result = sb.length(); result > 0; --result) {
        if (!Character::isWhitespace(sb.at(result - 1))) {
            break;
        }
    }
    return result;
}

}}}}
