#if !defined(CETTY_HANDLER_CODEC_HTTP_DEFAULTHTTPREQUEST_H)
#define CETTY_HANDLER_CODEC_HTTP_DEFAULTHTTPREQUEST_H

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

#include "cetty/handler/codec/http/HttpRequest.h"
#include "cetty/handler/codec/http/HttpMethod.h"
#include "cetty/handler/codec/http/DefaultHttpMessage.h"

namespace cetty { namespace handler { namespace codec { namespace http { 

/**
 * The default {@link HttpRequest} implementation.
 *
 * 
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2088 $, $Date: 2010-01-27 11:38:17 +0900 (Wed, 27 Jan 2010) $
 */
class DefaultHttpRequest : public HttpRequest {
public:
    /**
     * Creates a new instance.
     *
     * @param httpVersion the HTTP version of the request
     * @param method      the HTTP method of the request
     * @param uri         the URI or path of the request
     */
    DefaultHttpRequest(const HttpVersion& httpVersion,
                       const HttpMethod& method,
                       const std::string& uri)
        : message(httpVersion),
          httpHeader(message.defaultHttpHeader()),
          method(method),
          uri(uri) {
    }

    virtual const HttpMethod& getMethod() const {
        return method;
    }

    virtual void setMethod(const HttpMethod& method) {
        this->method = method;
    }

    virtual const std::string& getUri() const {
        return uri;
    }

    virtual void setUri(const std::string& uri) {
        this->uri = uri;
    }

    virtual bool isChunked() const {
        return message.isChunked();
    }
    virtual void setChunked(bool chunked) {
        message.setChunked(chunked);
    }

    virtual const ChannelBufferPtr& getContent() const {
        return message.getContent();
    }

    virtual void setContent(const ChannelBufferPtr& content) {
        message.setContent(content);        
    }

    virtual const HttpVersion& getProtocolVersion() const {
        return message.getProtocolVersion();
    }

    virtual void setProtocolVersion(const HttpVersion& version) {
        message.setProtocolVersion(version);
    }

    virtual std::string toString() const;

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

    void clear() {
        message.clear();
        uri.clear();
    }

private:
    DefaultHttpMessage message;
    DefaultHttpHeader& httpHeader;

    HttpMethod method;
    std::string uri;
};

typedef boost::intrusive_ptr<DefaultHttpRequest> DefaultHttpRequestPtr;

}}}}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_DEFAULTHTTPREQUEST_H)
