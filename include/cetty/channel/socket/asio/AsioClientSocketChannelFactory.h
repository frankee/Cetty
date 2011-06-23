#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOCLIENTSOCKETCHANNELFACTORY_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOCLIENTSOCKETCHANNELFACTORY_H

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

#include <vector>

#include "cetty/channel/IpAddress.h"
#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/socket/ClientSocketChannelFactory.h"
#include "cetty/channel/socket/asio/AsioServicePool.h"

#include "cetty/util/TimerFactory.h"

namespace cetty { namespace channel { namespace socket { namespace asio { 

using namespace cetty::channel;
using namespace cetty::util;

class AsioClientSocketPipelineSink;
class AsioIpAddressImplFactory;
class AsioTcpSocketAddressImplFactory;

/**
 * A {@link ClientSocketChannelFactory} which creates a client-side NIO-based
 * {@link SocketChannel}.  It utilizes the non-blocking I/O mode which was
 * introduced with NIO to serve many number of concurrent connections
 * efficiently.
 *
 * <h3>How threads work</h3>
 * <p>
 * There are two types of threads in a {@link NioClientSocketChannelFactory};
 * one is boss thread and the other is worker thread.
 *
 * <h4>Boss thread</h4>
 * <p>
 * One {@link NioClientSocketChannelFactory} has one boss thread.  It makes
 * a connection attempt on request.  Once a connection attempt succeeds,
 * the boss thread passes the connected {@link Channel} to one of the worker
 * threads that the {@link NioClientSocketChannelFactory} manages.
 *
 * <h4>Worker threads</h4>
 * <p>
 * One {@link NioClientSocketChannelFactory} can have one or more worker
 * threads.  A worker thread performs non-blocking read and write for one or
 * more {@link Channel}s in a non-blocking mode.
 *
 * <h3>Life cycle of threads and graceful shutdown</h3>
 * <p>
 * All threads are acquired from the {@link Executor}s which were specified
 * when a {@link NioClientSocketChannelFactory} was created.  A boss thread is
 * acquired from the <tt>bossExecutor</tt>, and worker threads are acquired from
 * the <tt>workerExecutor</tt>.  Therefore, you should make sure the specified
 * {@link Executor}s are able to lend the sufficient number of threads.
 * It is the best bet to specify @link Executors#newCachedThreadPool() a cached thread pool@endlink.
 * <p>
 * Both boss and worker threads are acquired lazily, and then released when
 * there's nothing left to process.  All the related resources such as
 * {@link Selector} are also released when the boss and worker threads are
 * released.  Therefore, to shut down a service gracefully, you should do the
 * following:
 *
 * <ol>
 * <li>close all channels created by the factory usually using
 *     {@link ChannelGroup#close()}, and</li>
 * <li>call {@link #releaseExternalResources()}.</li>
 * </ol>
 *
 * Please make sure not to shut down the executor until all channels are
 * closed.  Otherwise, you will end up with a {@link RejectedExecutionException}
 * and the related resources might not be released properly.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2200 $, $Date: 2010-02-23 14:42:39 +0900 (Tue, 23 Feb 2010) $
 *
 * @apiviz.landmark
 */

class AsioClientSocketChannelFactory
    : public cetty::channel::socket::ClientSocketChannelFactory {
public:
    /**
     * Creates a new instance.  Calling this constructor is same with calling
     * {@link #NioClientSocketChannelFactory(Executor, Executor, int)} with 2 *
     * the number of available processors in the machine.  The number of
     * available processors is obtained by {@link Runtime#availableProcessors()}.
     *
     * @param bossExecutor
     *        the {@link Executor} which will execute the boss thread
     * @param workerExecutor
     *        the {@link Executor} which will execute the I/O worker threads
     * @param workerCount
     *        the maximum number of I/O worker threads
     */
    AsioClientSocketChannelFactory(int ioThreadCount = 1);
    virtual ~AsioClientSocketChannelFactory();

    virtual Channel* newChannel(cetty::channel::ChannelPipeline* pipeline);

    virtual int  getIpProtocolVersion() const { return ipProtocol; }
    virtual void setIpProtocolVersion(int version) { ipProtocol = version; } 

    virtual void releaseExternalResources();

    AsioServicePool& getIOServicePool() { return ioServicePool; }

    void start();

private:
    void createSocketAddressImplFactory();
    void destorySocketAddressImplFactory();

private:
    int ipProtocol;

    AsioClientSocketPipelineSink* sink;
    AsioServicePool ioServicePool;

    TimerFactoryPtr timerFactory;

    std::vector<Channel*> clientChannels;

    AsioTcpSocketAddressImplFactory* socketAddressFactory;
    AsioIpAddressImplFactory* ipAddressFactory;
};

}}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOCLIENTSOCKETCHANNELFACTORY_H)
