#if !defined(CETTY_HANDLER_CODEC_HTTP_DEFAULTHTTPCHUNKTRAILER_H)
#define CETTY_HANDLER_CODEC_HTTP_DEFAULTHTTPCHUNKTRAILER_H

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

#include "cetty/handler/codec/http/HttpChunkTrailer.h"

namespace cetty { namespace handler { namespace codec { namespace http { 

class HttpHeader;

/**
 * The default {@link HttpChunkTrailer} implementation.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class DefaultHttpChunkTrailer : public HttpChunkTrailer {
public:
    DefaultHttpChunkTrailer();
    virtual ~DefaultHttpChunkTrailer();

    virtual HttpHeader& header();
    virtual const HttpHeader& header() const;

    virtual const ChannelBufferPtr& getContent() const;
    virtual void setContent(const ChannelBufferPtr& content);

    virtual const std::string& getHeader(const std::string& name) const;

    virtual StringList getHeaders(const std::string& name) const;
    virtual void getHeaders(const std::string& name, StringList& headers) const;

    virtual NameValueList getHeaders() const;
    virtual void getHeaders(NameValueList& nameValues) const;

    virtual bool containsHeader(const std::string& name) const;

    virtual StringList getHeaderNames() const;
    virtual void getHeaderNames(StringList& names) const;

    void addHeader(const std::string& name, const std::string& value);
    void addHeader(const std::string& name, int value);

    void setHeader(const std::string& name, const std::string& value);
    void setHeader(const std::string& name, int value);

    void setHeader(const std::string& name, const StringList& values);
    void setHeader(const std::string& name, const IntList& values);

    virtual void removeHeader(const std::string& name);
    virtual void removeHeader(const std::string& name, const std::string& value);

    virtual void clearHeaders();

    virtual std::string toString() const;

private:
    DefaultHttpChunkTrailer(const DefaultHttpChunkTrailer&);
    DefaultHttpChunkTrailer& operator=(const DefaultHttpChunkTrailer&);

private:
    HttpHeader* httpHeader;
};


}}}}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_DEFAULTHTTPCHUNKTRAILER_H)
