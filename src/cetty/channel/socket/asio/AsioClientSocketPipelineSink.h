#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOCLIENTSOCKETPIPELINESINK_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOCLIENTSOCKETPIPELINESINK_H

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

#include <boost/asio.hpp>
#include "cetty/channel/ChannelFuture.h"
#include "cetty/channel/AbstractChannelSink.h"

#include "cetty/buffer/ChannelBuffer.h"

namespace cetty { namespace channel {
class SocketAddress;
class ChannelPipeline;
class MessageEvent;
}}

namespace cetty { namespace logging {
class InternalLogger;
}}

namespace cetty { namespace channel { namespace socket { namespace asio { 

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::logging;

class AsioSocketChannel;
class AsioClientSocketChannel;
class AsioServicePool;

/**
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */
class AsioClientSocketPipelineSink : public ::cetty::channel::AbstractChannelSink {
public:
    AsioClientSocketPipelineSink(AsioServicePool& ioServicePool)
        : ioServicePool(ioServicePool) {
    }

    virtual ~AsioClientSocketPipelineSink() {}

    virtual void writeRequested(const cetty::channel::ChannelPipeline& pipeline,
                                const cetty::channel::MessageEvent& e);

    virtual void stateChangeRequested(const cetty::channel::ChannelPipeline& pipeline,
                                      const cetty::channel::ChannelStateEvent& e);

private:
    void connect(AsioSocketChannel& channel,
                 const ChannelFuturePtr& cf,
                 const SocketAddress& remoteAddress);

    void handleStateChange(AsioSocketChannel& channel, const ChannelStateEvent& evt);

private:
    static InternalLogger* logger;
    AsioServicePool& ioServicePool;
};

}}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOCLIENTSOCKETPIPELINESINK_H)
