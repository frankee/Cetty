#if !defined(CETTY_HANDLER_CODEC_HTTP_DEFAULTHTTPCHUNK_H)
#define CETTY_HANDLER_CODEC_HTTP_DEFAULTHTTPCHUNK_H

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

#include "cetty/handler/codec/http/HttpChunk.h"

namespace cetty { namespace handler { namespace codec { namespace http { 

/**
 * The default {@link HttpChunk} implementation.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */

class DefaultHttpChunk : public HttpChunk {
public:
    /**
     * Creates a new instance with the specified chunk content. If an empty
     * buffer is specified, this chunk becomes the 'end of content' marker.
     */
    DefaultHttpChunk(const ChannelBufferPtr& content) {
        setContent(content);
    }
    virtual ~DefaultHttpChunk() {}

    virtual const ChannelBufferPtr& getContent() const {
        return content;
    }

    virtual void setContent(const ChannelBufferPtr& content);

    virtual bool isLast() const { return last; }

    virtual std::string toString() const { return "DefaultHttpChunk"; }

private:
    ChannelBufferPtr content;
    bool last;
};

}}}}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_DEFAULTHTTPCHUNK_H)
