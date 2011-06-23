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

#include "cetty/handler/codec/embedder/AbstractCodecEmbedder.h"
#include "cetty/channel/Channel.h"
#include "cetty/channel/Channels.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ExceptionEvent.h"
#include "cetty/channel/ChannelConfig.h"
#include "cetty/channel/ChannelPipelineException.h"
#include "cetty/util/Integer.h"
#include "cetty/util/Exception.h"

#include "cetty/handler/codec/embedder/EmbeddedChannel.h"
#include "cetty/handler/codec/embedder/EmbeddedChannelFactory.h"
#include "cetty/handler/codec/embedder/CodecEmbedderException.h"

namespace cetty { namespace handler { namespace codec { namespace embedder {

using namespace cetty::channel;
using namespace cetty::util;

const Integer EmbeddedChannel::DUMMY_ID(0);

bool AbstractCodecEmbedder::finish() {
    Channels::close(*channel);
    Channels::fireChannelDisconnected(*channel);
    Channels::fireChannelUnbound(*channel);
    Channels::fireChannelClosed(*channel);
    return !productQueue.empty();
}

cetty::channel::ChannelMessage AbstractCodecEmbedder::poll() {
    ChannelMessage msg = productQueue.front();
    productQueue.pop_front();
    return msg;
}

const ChannelMessage& AbstractCodecEmbedder::peek() const {
    return productQueue.front();
}

std::vector<ChannelMessage> AbstractCodecEmbedder::pollAll() {
    std::vector<ChannelMessage> msgs(productQueue.begin(), productQueue.end());
    productQueue.clear();
    return msgs;
}

int AbstractCodecEmbedder::size() const {
    return (int)productQueue.size();
}

AbstractCodecEmbedder::AbstractCodecEmbedder(const std::vector<ChannelHandlerPtr>& handlers) {
    init(handlers);
}

AbstractCodecEmbedder::AbstractCodecEmbedder(ChannelBufferFactory* bufferFactory,
                                             const std::vector<ChannelHandlerPtr>& handlers) {
    init(handlers);
    getChannel().getConfig().setBufferFactory(bufferFactory);
}

AbstractCodecEmbedder::~AbstractCodecEmbedder() {
    uninit();
}

void AbstractCodecEmbedder::init(const std::vector<ChannelHandlerPtr>& handlers) {
    pipeline = new EmbeddedChannelPipeline();
    sink = boost::intrusive_ptr<EmbeddedChannelSink>(new EmbeddedChannelSink(productQueue));
    channel = new EmbeddedChannel(pipeline, sink.get());

    BOOST_ASSERT(pipeline && sink && channel && "MEMORY ALLOC FAILED");
    configurePipeline(handlers);
    fireInitialEvents();
}

void AbstractCodecEmbedder::fireInitialEvents() {
    // Fire the typical initial events.
    Channels::fireChannelOpen(*channel);
    Channels::fireChannelBound(*channel, channel->getLocalAddress());
    Channels::fireChannelConnected(*channel, channel->getRemoteAddress());
}

void AbstractCodecEmbedder::uninit() {
    if (channel) delete channel;
}

void AbstractCodecEmbedder::configurePipeline(const std::vector<ChannelHandlerPtr>& handlers) {
    if (handlers.empty()) {
        throw InvalidArgumentException("handlers should contain at least one ");
    }

    for (size_t i = 0; i < handlers.size(); ++i) {
        pipeline->addLast(Integer::toString(i), handlers[i]);
    }

    pipeline->addLast("SINK", sink);
}

void AbstractCodecEmbedder::EmbeddedChannelSink::exceptionCaught(const ChannelPipeline& pipeline,
                                                                 const ChannelEvent& e,
                                                                 const ChannelPipelineException& cause) {
    const Exception* excp = cause.getNested();
    if (excp) {
        throw CodecEmbedderException(excp->what(), excp->getCode());
    }
    throw CodecEmbedderException(cause.what(), cause.getCode());
}

void AbstractCodecEmbedder::EmbeddedChannelSink::handleMessageEvent(const MessageEvent& e) {
    productQueue.push_front(e.getMessage());
}

void AbstractCodecEmbedder::EmbeddedChannelSink::handleExceptionEvent(const ExceptionEvent& e) {
    throw CodecEmbedderException(e.getCause().what(), e.getCause().getCode());
}

void AbstractCodecEmbedder::EmbeddedChannelPipeline::notifyHandlerException(const ChannelEvent& evt,
                                                                            const Exception& e) const {
    const ChannelPipelineException* pipelineException =
        dynamic_cast<const ChannelPipelineException*>(&e);
    
    while (pipelineException && pipelineException->getNested()) {
        pipelineException =
            dynamic_cast<const ChannelPipelineException*>(pipelineException->getNested());
    }

    const CodecEmbedderException* codecException =
        dynamic_cast<const CodecEmbedderException*>(pipelineException);
    if (codecException) {
        codecException->rethrow();
    }
    else {
        throw CodecEmbedderException(pipelineException->what(),
                                     pipelineException->getCode());
    }
}

}}}}