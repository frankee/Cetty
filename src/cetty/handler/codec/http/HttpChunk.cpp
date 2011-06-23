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

#include "cetty/handler/codec/http/HttpHeader.h"
#include "cetty/handler/codec/http/HttpChunk.h"
#include "cetty/handler/codec/http/HttpChunkTrailer.h"
#include "cetty/buffer/ChannelBuffer.h"
#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/util/Exception.h"

namespace cetty { namespace handler { namespace codec { namespace http {

using namespace cetty::buffer;
using namespace cetty::util;

class NullHttpHeader : public HttpHeader {
public:
    NullHttpHeader() {}
    virtual ~NullHttpHeader() {}

public:
    virtual const std::string& get(const std::string& name) const {
        return EMPTY_STRING;
    }
    virtual StringList gets(const std::string& name) const {
        return StringList();
    }
    virtual void gets(const std::string& name, StringList& headers) const {}
    
    virtual NameValueList gets() const { return NameValueList(); }
    virtual void gets(NameValueList& nameValues) const {}

    virtual bool contains(const std::string& name) const { return false; }

    virtual StringList getNames() const { return StringList(); }
    virtual void getNames(StringList& names) const {}

    virtual void add(const std::string& name, const std::string& value) {
        throw IllegalStateException("read-only");
    }
    virtual void add(const std::string& name, int value) {
        throw IllegalStateException("read-only");
    }

    virtual void set(const std::string& name, const std::string& value) {
        throw IllegalStateException("read-only");
    }
    virtual void set(const std::string& name, int value) {
        throw IllegalStateException("read-only");
    }

    virtual void set(const std::string& name, const StringList& values) {
        throw IllegalStateException("read-only");
    }
    virtual void set(const std::string& name, const IntList& values) {
        throw IllegalStateException("read-only");
    }

    virtual void remove(const std::string& name) {}
    virtual void remove(const std::string& name, const std::string& value) {}

    virtual void clear() {}

private:
    static const std::string EMPTY_STRING;
};

const std::string NullHttpHeader::EMPTY_STRING;

class LastHttpChunkTrailer : public HttpChunkTrailer {
public:
    LastHttpChunkTrailer() {}
    virtual ~LastHttpChunkTrailer() {}

    virtual const ChannelBufferPtr& getContent() const {
        return ChannelBuffers::EMPTY_BUFFER;
    }

    virtual void setContent(const ChannelBufferPtr& content) {
        throw IllegalStateException("read-only");
    }

    virtual HttpHeader& header() { return nullHeader; }
    virtual const HttpHeader& header() const { return nullHeader; }

    virtual const std::string& getHeader(const std::string& name) const {
        return nullHeader.get(name);
    }

    virtual StringList getHeaders(const std::string& name) const {
        return nullHeader.gets(name);
    }
    virtual void getHeaders(const std::string& name, StringList& headers) const {
        nullHeader.gets(name, headers);
    }

    virtual NameValueList getHeaders() const {
        return nullHeader.gets();
    }
    virtual void getHeaders(NameValueList& nameValues) const {
        nullHeader.gets(nameValues);
    }

    virtual bool containsHeader(const std::string& name) const {
        return nullHeader.contains(name);
    }

    virtual StringList getHeaderNames() const {
        return nullHeader.getNames();
    }
    virtual void getHeaderNames(StringList& names) const {
        nullHeader.getNames(names);
    }

    virtual void addHeader(const std::string& name, const std::string& value) {
        nullHeader.add(name, value);
    }
    virtual void addHeader(const std::string& name, int value) {
        nullHeader.add(name, value);
    }

    virtual void setHeader(const std::string& name, const std::string& value) {
        nullHeader.set(name, value);
    }
    virtual void setHeader(const std::string& name, int value) {
        nullHeader.set(name, value);
    }

    virtual void setHeader(const std::string& name, const StringList& values) {
        nullHeader.set(name, values);
    }
    virtual void setHeader(const std::string& name, const IntList& values) {
        nullHeader.set(name, values);
    }

    virtual void removeHeader(const std::string& name) {
        nullHeader.remove(name);
    }

    virtual void removeHeader(const std::string& name, const std::string& value) {
        nullHeader.remove(name, value);
    }

    virtual void clearHeaders() {
        nullHeader.clear();
    }

    virtual std::string toString() const {
        return "LastHttpChunkTrailer";
    }

private:
    NullHttpHeader nullHeader;
};

/**
 * The 'end of content' marker in chunked encoding.
 */
boost::intrusive_ptr<HttpChunk> HttpChunk::LAST_CHUNK
                                        = HttpChunkPtr(new LastHttpChunkTrailer);

}}}}