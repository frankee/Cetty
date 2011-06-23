#if !defined(CETTY_HANDLER_CODEC_EMBEDDER_EMBEDDEDCHANNEL_H)
#define CETTY_HANDLER_CODEC_EMBEDDER_EMBEDDEDCHANNEL_H

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

#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/AbstractChannel.h"
#include "cetty/channel/DefaultChannelConfig.h"

#include "cetty/handler/codec/embedder/EmbeddedChannelFactory.h"

namespace cetty { namespace handler { namespace codec { namespace embedder { 

using namespace cetty::channel;

/**
 * TODO Make EmbeddedChannel implement ChannelConfig and ChannelSink to reduce overhead.
 * TODO Do not extend AbstractChannel to reduce overhead and remove the internal-use-only constructor in AbstractChannel.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2280 $, $Date: 2010-05-19 15:29:43 +0900 (Wed, 19 May 2010) $
 */

class EmbeddedChannel : public cetty::channel::AbstractChannel {
public:
    EmbeddedChannel(ChannelPipeline* pipeline, ChannelSink* sink)
        : AbstractChannel(DUMMY_ID, NULL, EmbeddedChannelFactory::INSTANCE, pipeline, sink) {
    }
    virtual ~EmbeddedChannel() {}

    virtual ChannelConfig& getConfig() {
        return config;
    }
    virtual const ChannelConfig& getConfig() const {
        return config;
    }

    virtual const SocketAddress& getLocalAddress() const {
        return SocketAddress::NULL_ADDRESS;
    }

    virtual const SocketAddress& getRemoteAddress() const {
        return SocketAddress::NULL_ADDRESS;
    }
    
    virtual bool isBound() const {
        return true;
    }

    virtual bool isConnected() const {
        return true;
    }

private:
    static const Integer DUMMY_ID;
    DefaultChannelConfig config;
};


}}}}

#endif //#if !defined(CETTY_HANDLER_CODEC_EMBEDDER_EMBEDDEDCHANNEL_H)
