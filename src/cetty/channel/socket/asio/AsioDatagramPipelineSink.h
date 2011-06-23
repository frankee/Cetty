#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIODATAGRAMPIPELINESINK_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIODATAGRAMPIPELINESINK_H

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

#include "cetty/channel/AbstractChannelSink.h"


namespace cetty { namespace channel {
class ChannelFuture;
class SocketAddress;
}}

namespace cetty { namespace channel { namespace socket { namespace asio { 

using namespace cetty::channel;

class AsioDatagramChannel;
class AsioServicePool;

/**
 * Receives downstream events from a {@link ChannelPipeline}.  It contains
 * an array of I/O workers.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author Daniel Bevenius (dbevenius@jboss.com)
 *
 * @version $Rev: 2217 $, $Date: 2010-03-16 18:39:56 +0900 (Tue, 16 Mar 2010) $
 */
class AsioDatagramPipelineSink : public cetty::channel::AbstractChannelSink {
public:
    /**
     * Creates a new {@link NioDatagramPipelineSink} with a the number of {@link NioDatagramWorker}s specified in workerCount.
     * The {@link NioDatagramWorker}s take care of reading and writing for the {@link NioDatagramChannel}.
     *
     * @param workerExecutor
     *        the {@link Executor} that will run the {@link NioDatagramWorker}s
     *        for this sink
     * @param workerCount
     *        the number of {@link NioDatagramWorker}s for this sink
     */
    AsioDatagramPipelineSink(AsioServicePool& servicePool)
        : ioServicePool(servicePool) {
    }

    /**
     * Handle downstream MessageEvent.
     *
     * @param pipeline the {@link ChannelPipeline} that passes down the
     *                 downstream event.
     * @param e The MessageEvent.
     */
    virtual void writeRequested(const cetty::channel::ChannelPipeline& pipeline,
                                const cetty::channel::MessageEvent& e);

    /**
     * Handle downstream ChannelStateEvent.
     *
     * @param pipeline the {@link ChannelPipeline} that passes down the
     *                 downstream event.
     * @param e The ChannelStateEvent.
     */
    virtual void stateChangeRequested(const cetty::channel::ChannelPipeline& pipeline,
                                      const cetty::channel::ChannelStateEvent& e);

private:
    void handleStateChange(AsioDatagramChannel& channel, const ChannelStateEvent& evt);

    void close(AsioDatagramChannel& channel,
               const ChannelFuturePtr& future);
    
    void bind(AsioDatagramChannel& channel,
              const ChannelFuturePtr& future,
              const SocketAddress& address);
    
    void connect(AsioDatagramChannel& channel,
                 const ChannelFuturePtr& future,
                 const SocketAddress& remoteAddress);
    
    void startChannel(AsioDatagramChannel &channel);

private:
    int ioThreadCount;
    AsioServicePool& ioServicePool;
};

}}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIODATAGRAMPIPELINESINK_H)
