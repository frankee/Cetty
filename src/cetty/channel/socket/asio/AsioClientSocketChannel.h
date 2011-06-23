#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOCLIENTSOCKETCHANNEL_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOCLIENTSOCKETCHANNEL_H

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

#include "cetty/channel/Channels.h"
#include "cetty/channel/socket/SocketChannel.h"
#include "cetty/channel/socket/asio/AsioSocketChannel.h"

namespace cetty { namespace logging {
class InternalLogger;
}}

namespace cetty { namespace channel { namespace socket { namespace asio {

using namespace cetty::channel;
using namespace cetty::channel::socket;
using namespace cetty::logging;

/**
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 *
 */

class AsioClientSocketChannel : public AsioSocketChannel {
public:
    AsioClientSocketChannel(
            ChannelFactory* factory,
            ChannelPipeline* pipeline,
            ChannelSink* sink,
            AsioServicePool::IOService& ioService,
            const boost::thread::id& id)
            : AsioSocketChannel(NULL, factory, pipeline, sink, ioService, id) {
        Channels::fireChannelOpen(*this);
    }

    virtual ~AsioClientSocketChannel() {}

private:
    static InternalLogger* logger;

    ChannelFuturePtr connectFuture;
    bool boundManually;

    // Does not need to be volatile as it's accessed by only one thread.
    long connectDeadlineMills;
};

}}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOCLIENTSOCKETCHANNEL_H)
