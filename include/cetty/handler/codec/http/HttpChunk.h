#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCHUNK_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPCHUNK_H

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
#include "cetty/util/ReferenceCounter.h"

namespace cetty { namespace handler { namespace codec { namespace http {

using namespace cetty::buffer;

/**
 * An HTTP chunk which is used for HTTP chunked transfer-encoding.
 * {@link HttpMessageDecoder} generates {@link HttpChunk} after
 * {@link HttpMessage} when the content is large or the encoding of the content
 * is 'chunked.  If you prefer not to receive {@link HttpChunk} in your handler,
 * please {@link HttpChunkAggregator} after {@link HttpMessageDecoder} in the
 * {@link ChannelPipeline}.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 *
 * @apiviz.landmark
 */

class HttpChunk : public cetty::util::ReferenceCounter<HttpChunk> {
public:
    virtual ~HttpChunk() {}

    /**
     * The 'end of content' marker in chunked encoding.
     */
    static boost::intrusive_ptr<HttpChunk> LAST_CHUNK;

    /**
     * Returns <tt>true</tt> if and only if this chunk is the 'end of content'
     * marker.
     */
    virtual bool isLast() const = 0;

    /**
     * Returns the content of this chunk.  If this is the 'end of content'
     * marker, {@link ChannelBuffers#EMPTY_BUFFER} will be returned.
     */
    virtual const ChannelBufferPtr& getContent() const = 0;

    /**
     * Sets the content of this chunk.  If an empty buffer is specified,
     * this chunk becomes the 'end of content' marker.
     */
    virtual void setContent(const ChannelBufferPtr& content) = 0;


    virtual std::string toString() const = 0;
};

typedef boost::intrusive_ptr<HttpChunk> HttpChunkPtr;

}}}}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCHUNK_H)
