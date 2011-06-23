#if !defined(CETTY_HANDLER_CODEC_HTTP_DEFAULTHTTPMESSAGE_H)
#define CETTY_HANDLER_CODEC_HTTP_DEFAULTHTTPMESSAGE_H

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

#include "cetty/handler/codec/http/HttpVersion.h"
#include "cetty/handler/codec/http/HttpMessage.h"
#include "cetty/handler/codec/http/DefaultHttpHeader.h"

namespace cetty { namespace buffer {
class ChannelBuffer;
}}

namespace cetty { namespace handler { namespace codec { namespace http {

/**
 * The default {@link HttpMessage} implementation.
 *
 *
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2088 $, $Date: 2010-01-27 11:38:17 +0900 (Wed, 27 Jan 2010) $
 */

class DefaultHttpMessage : public HttpMessage {
public:
    DefaultHttpMessage();
    DefaultHttpMessage(const HttpVersion& version);
    virtual ~DefaultHttpMessage() {}

    void clear() {
        chunked = false;
        httpHeader.clear();
        content.reset();
    }

    virtual HttpHeader& header() {
        return httpHeader;
    }

    virtual const HttpHeader& header() const {
        return httpHeader;
    }

    virtual const std::string& getHeader(const std::string& name) const {
        return httpHeader.get(name);
    }

    virtual StringList getHeaders(const std::string& name) const {
        return httpHeader.gets(name);
    }
    virtual void getHeaders(const std::string& name, StringList& headers) const {
        httpHeader.gets(name, headers);
    }

    virtual NameValueList getHeaders() const {
        return httpHeader.gets();
    }
    virtual void getHeaders(NameValueList& nameValues) const {
        httpHeader.gets(nameValues);
    }

    virtual bool containsHeader(const std::string& name) const {
        return httpHeader.contains(name);
    }

    virtual StringList getHeaderNames() const {
        return httpHeader.getNames();
    }
    virtual void getHeaderNames(StringList& names) const {
        httpHeader.getNames(names);
    }

    virtual void addHeader(const std::string& name, const std::string& value) {
        httpHeader.add(name, value);
    }
    virtual void addHeader(const std::string& name, int value) {
        httpHeader.add(name, value);
    }

    virtual void setHeader(const std::string& name, const std::string& value) {
        httpHeader.set(name, value);
    }
    virtual void setHeader(const std::string& name, int value) {
        httpHeader.set(name, value);
    }

    virtual void setHeader(const std::string& name, const StringList& values) {
        httpHeader.set(name, values);
    }
    virtual void setHeader(const std::string& name, const IntList& values) {
        httpHeader.set(name, values);
    }

    virtual void removeHeader(const std::string& name) {
        httpHeader.remove(name);
    }

    virtual void removeHeader(const std::string& name, const std::string& value) {
        httpHeader.remove(name, value);
    }

    virtual void clearHeaders() {
        httpHeader.clear();
    }

    virtual bool isChunked() const;

    virtual void setChunked(bool chunked);

    virtual const ChannelBufferPtr& getContent() const {
        return content;
    }

    virtual void setContent(const ChannelBufferPtr& content);

    virtual const HttpVersion& getProtocolVersion() const {
        return version;
    }

    virtual void setProtocolVersion(const HttpVersion& version) {
        this->version = version;
    }

    virtual std::string toString() const;

//protected: internal use here.
    void appendHeaders(std::string& buf) const;

    DefaultHttpHeader& defaultHttpHeader() { return httpHeader; }

private:
    bool chunked;
    HttpVersion version;
    DefaultHttpHeader httpHeader;

    ChannelBufferPtr content;
};

}}}}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_DEFAULTHTTPMESSAGE_H)
