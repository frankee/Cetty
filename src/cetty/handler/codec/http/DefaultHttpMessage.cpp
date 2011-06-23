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
#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/util/StringUtil.h"
#include "cetty/util/internal/ConversionUtil.h"
#include "cetty/util/Exception.h"
#include "cetty/handler/codec/http/HttpVersion.h"
#include "cetty/handler/codec/http/DefaultHttpMessage.h"
#include "cetty/handler/codec/http/HttpCodecUtil.h"
#include "cetty/handler/codec/http/HttpHeaders.h"

namespace cetty { namespace handler { namespace codec { namespace http {

using namespace cetty::buffer;
using namespace cetty::util;
using namespace cetty::util::internal;

DefaultHttpMessage::DefaultHttpMessage(const HttpVersion& version)
    : chunked(false),
      version(version),
      content(ChannelBuffers::EMPTY_BUFFER) {
}

DefaultHttpMessage::DefaultHttpMessage()
    : chunked(false),
      version(HttpVersion::HTTP_1_1),
      content(ChannelBuffers::EMPTY_BUFFER) {
}

bool DefaultHttpMessage::isChunked() const {
    if (chunked) {
        return true;
    }
    else {
        return HttpCodecUtil::isTransferEncodingChunked(*this);
    }
}

void DefaultHttpMessage::setChunked(bool chunked) {
    this->chunked = chunked;
    if (chunked) {
        setContent(ChannelBuffers::EMPTY_BUFFER);
    }

}

void DefaultHttpMessage::setContent(const ChannelBufferPtr& content) {
    if (!content) {
        this->content = ChannelBuffers::EMPTY_BUFFER;
    }
    if (content->readable() && isChunked()) {
        throw InvalidArgumentException(
            "non-empty content disallowed if this.chunked == true");
    }
    this->content = content;
}

std::string DefaultHttpMessage::toString() const {
    std::string buf;
    buf.reserve(2048);

    buf.append("HttpMessage ");
    buf.append("(version: ");
    buf.append(getProtocolVersion().getText());
    buf.append(", keepAlive: ");
    buf.append(ConversionUtil::toString(HttpHeaders::isKeepAlive(*this)));
    buf.append(", chunked: ");
    buf.append(ConversionUtil::toString(isChunked()));
    buf.append(")");
    buf.append(StringUtil::NEWLINE);
    appendHeaders(buf);

    // Remove the last newline.
    buf[buf.length() - StringUtil::NEWLINE.length()] = '\0';
    return buf;
}

void DefaultHttpMessage::appendHeaders(std::string& buf) const {
    HttpHeader::NameValueList headers = httpHeader.gets();

    for (size_t i = 0; i < headers.size(); ++i) {
        buf.append(headers[i].first);
        buf.append(": ");
        buf.append(headers[i].second);
        buf.append(StringUtil::NEWLINE);
    }
}

}}}}
