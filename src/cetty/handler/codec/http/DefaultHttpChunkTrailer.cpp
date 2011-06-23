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

#include <boost/assert.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "cetty/handler/codec/http/HttpHeaders.h"
#include "cetty/handler/codec/http/DefaultHttpHeader.h"
#include "cetty/handler/codec/http/DefaultHttpChunkTrailer.h"
#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/util/Exception.h"

namespace cetty { namespace handler { namespace codec { namespace http {

using namespace cetty::buffer;
using namespace cetty::util;

class HttpChunkTrailerHeader : public DefaultHttpHeader {
public:
    HttpChunkTrailerHeader() {}
    virtual ~HttpChunkTrailerHeader() {}

    virtual void validateHeaderName(const std::string& name) {
        DefaultHttpHeader::validateHeaderName(name);
        if (boost::algorithm::iequals(name, HttpHeaders::Names::CONTENT_LENGTH) ||
            boost::algorithm::iequals(name, HttpHeaders::Names::TRANSFER_ENCODING) ||
            boost::algorithm::iequals(name, HttpHeaders::Names::TRAILER)) {
                throw InvalidArgumentException(
                    std::string("prohibited trailing header: ") + name);
        }
    }
};

DefaultHttpChunkTrailer::DefaultHttpChunkTrailer()
    : httpHeader(new HttpChunkTrailerHeader) {

    BOOST_ASSERT(httpHeader);
}

DefaultHttpChunkTrailer::~DefaultHttpChunkTrailer() {
    if (httpHeader) {
        delete httpHeader;
    }
}

HttpHeader& DefaultHttpChunkTrailer::header() {
    return *httpHeader;
}

const HttpHeader& DefaultHttpChunkTrailer::header() const {
    return *httpHeader;
}

const ChannelBufferPtr& DefaultHttpChunkTrailer::getContent() const {
    return ChannelBuffers::EMPTY_BUFFER;
}

void DefaultHttpChunkTrailer::setContent(const ChannelBufferPtr& content) {
    throw IllegalStateException("read-only");
}

const std::string& DefaultHttpChunkTrailer::getHeader(const std::string& name) const {
    return httpHeader->get(name);
}

HttpChunkTrailer::StringList DefaultHttpChunkTrailer::getHeaders(const std::string& name) const {
    return httpHeader->gets(name);
}

void DefaultHttpChunkTrailer::getHeaders(const std::string& name, StringList& headers) const {
    httpHeader->gets(name, headers);
}

HttpChunkTrailer::NameValueList DefaultHttpChunkTrailer::getHeaders() const {
    return httpHeader->gets();
}

void DefaultHttpChunkTrailer::getHeaders(NameValueList& nameValues) const {
    httpHeader->gets(nameValues);
}

bool DefaultHttpChunkTrailer::containsHeader(const std::string& name) const {
    return httpHeader->contains(name);
}

HttpChunkTrailer::StringList DefaultHttpChunkTrailer::getHeaderNames() const {
    return httpHeader->getNames();
}

void DefaultHttpChunkTrailer::getHeaderNames(StringList& names) const {
    httpHeader->getNames(names);
}

void DefaultHttpChunkTrailer::addHeader(const std::string& name, const std::string& value) {
    httpHeader->add(name, value);
}

void DefaultHttpChunkTrailer::addHeader(const std::string& name, int value) {
    httpHeader->add(name, value);
}

void DefaultHttpChunkTrailer::setHeader(const std::string& name, const std::string& value) {
    httpHeader->set(name, value);
}

void DefaultHttpChunkTrailer::setHeader(const std::string& name, int value) {
    httpHeader->set(name, value);
}

void DefaultHttpChunkTrailer::setHeader(const std::string& name, const StringList& values) {
    httpHeader->set(name, values);
}

void DefaultHttpChunkTrailer::setHeader(const std::string& name, const IntList& values) {
    httpHeader->set(name, values);
}

void DefaultHttpChunkTrailer::removeHeader(const std::string& name) {
    httpHeader->remove(name);
}

void DefaultHttpChunkTrailer::removeHeader(const std::string& name, const std::string& value) {
    httpHeader->remove(name, value);
}

void DefaultHttpChunkTrailer::clearHeaders() {
    httpHeader->clear();
}

std::string DefaultHttpChunkTrailer::toString() const {
    return "DefaultHttpChunkTrailer";
}

}}}}
