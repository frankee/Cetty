#if !defined(CETTY_BOOTSTRAP_CONNECTIONLESSBOOTSTRAP_H)
#define CETTY_BOOTSTRAP_CONNECTIONLESSBOOTSTRAP_H

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

#include "cetty/bootstrap/Bootstrap.h"
#include "cetty/channel/ChannelFuture.h"

namespace cetty { namespace channel {
class Channel;
class ChannelFuture;
class ChannelFactory;
class SocketAddress;
}}

namespace cetty { namespace bootstrap {

using namespace cetty::channel;

/**
 * A helper class which creates a new server-side {@link Channel} for a
 * connectionless transport.
 *
 * <h3>Only for connectionless transports</h3>
 *
 * This bootstrap is for connectionless transports only such as UDP/IP.
 * Use {@link ServerBootstrap} instead for connection oriented transports.
 * Do not use this helper if you are using a connection oriented transport such
 * as TCP/IP and local transport which accepts an incoming connection and lets
 * the accepted child channels handle received messages.
 *
 * <h3>Configuring channels</h3>
 *
 * {@link #setOption(String, Object) Options} are used to configure a channel:
 *
 * <pre>
 * {@link ConnectionlessBootstrap} b = ...;
 *
 * // Options for a new channel
 * b.setOption("localAddress", new {@link InetSocketAddress}(8080));
 * b.setOption("tcpNoDelay", true);
 * b.setOption("receiveBufferSize", 1048576);
 * </pre>
 *
 * For the detailed list of available options, please refer to
 * {@link ChannelConfig} and its sub-types.
 *
 * <h3>Configuring a channel pipeline</h3>
 *
 * Every channel has its own {@link ChannelPipeline} and you can configure it
 * in two ways.
 *
 * The recommended approach is to specify a {@link ChannelPipelineFactory} by
 * calling {@link #setPipelineFactory(ChannelPipelineFactory)}.
 *
 * <pre>
 * {@link ConnectionlessBootstrap} b = ...;
 * b.setPipelineFactory(new MyPipelineFactory());
 *
 * public class MyPipelineFactory implements {@link ChannelPipelineFactory} {
 *   public {@link ChannelPipeline} getPipeline() throws Exception {
 *     // Create and configure a new pipeline for a new channel.
 *     {@link ChannelPipeline} p = {@link Channels}.pipeline();
 *     p.addLast("encoder", new EncodingHandler());
 *     p.addLast("decoder", new DecodingHandler());
 *     p.addLast("logic",   new LogicHandler());
 *     return p;
 *   }
 * }
 * </pre>

 * <p>
 * The alternative approach, which works only in a certain situation, is to use
 * the default pipeline and let the bootstrap to shallow-copy the default
 * pipeline for each new channel:
 *
 * <pre>
 * {@link ConnectionlessBootstrap} b = ...;
 * {@link ChannelPipeline} p = b.getPipeline();
 *
 * // Add handlers to the default pipeline.
 * p.addLast("encoder", new EncodingHandler());
 * p.addLast("decoder", new DecodingHandler());
 * p.addLast("logic",   new LogicHandler());
 * </pre>
 *
 * Please note 'shallow-copy' here means that the added {@link ChannelHandler}s
 * are not cloned but only their references are added to the new pipeline.
 * Therefore, you cannot use this approach if you are going to open more than
 * one {@link Channel}s or run a server that accepts incoming connections to
 * create its child channels.
 *
 * <h3>Applying different settings for different {@link Channel}s</h3>
 *
 * {@link ConnectionlessBootstrap} is just a helper class.  It neither
 * allocates nor manages any resources.  What manages the resources is the
 * {@link ChannelFactory} implementation you specified in the constructor of
 * {@link ConnectionlessBootstrap}.  Therefore, it is OK to create as
 * many {@link ConnectionlessBootstrap} instances as you want with the same
 * {@link ChannelFactory} to apply different settings for different
 * {@link Channel}s.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 */

class ConnectionlessBootstrap : public Bootstrap {
public:
    /**
     * Creates a new instance with no {@link ChannelFactory} set.
     * {@link #setFactory(ChannelFactory)} must be called before any I/O
     * operation is requested.
     */
    ConnectionlessBootstrap() {
    }

    /**
     * Creates a new instance with the specified initial {@link ChannelFactory}.
     */
    ConnectionlessBootstrap(const ChannelFactoryPtr& channelFactory)
        : Bootstrap(channelFactory) {
    }

    /**
     * Creates a new channel which is bound to the local address which was
     * specified in the current <tt>"localAddress"</tt> option.  This method is
     * similar to the following code:
     *
     * <pre>
     * {@link ConnectionlessBootstrap} b = ...;
     * b.bind(b.getOption("localAddress"));
     * </pre>
     *
     * @return a new bound channel which accepts incoming connections
     *
     * @throws IllegalStateException
     *         if <tt>"localAddress"</tt> option was not set
     * @throws ClassCastException
     *         if <tt>"localAddress"</tt> option's value is
     *         neither a {@link SocketAddress} nor <tt>null</tt>
     * @throws ChannelException
     *         if failed to create a new channel and
     *                      bind it to the local address
     */
    Channel* bind();

    /**
     * Creates a new channel which is bound to the specified local address.
     *
     * @return a new bound channel which accepts incoming connections
     *
     * @throws ChannelException
     *         if failed to create a new channel and
     *                      bind it to the local address
     */
    Channel* bind(const SocketAddress& localAddress);

    /**
     * Creates a new connected channel with the current <tt>"remoteAddress"</tt>
     * and <tt>"localAddress"</tt> option.  If the <tt>"localAddress"</tt> option
     * is not set, the local address of a new channel is determined
     * automatically. This method is similar to the following code:
     *
     * <pre>
     * {@link ConnectionlessBootstrap} b = ...;
     * b.connect(b.getOption("remoteAddress"), b.getOption("localAddress"));
     * </pre>
     *
     * @return a future object which notifies when the creation of the connected
     *         channel succeeds or fails
     *
     * @throws IllegalStateException
     *         if <tt>"remoteAddress"</tt> option was not set
     * @throws ClassCastException
     *         if <tt>"remoteAddress"</tt> or <tt>"localAddress"</tt> option's
     *            value is neither a {@link SocketAddress} nor <tt>null</tt>
     * @throws ChannelPipelineException
     *         if this bootstrap's {@link #setPipelineFactory(ChannelPipelineFactory) pipelineFactory}
     *            failed to create a new {@link ChannelPipeline}
     */
    ChannelFuturePtr connect();

    /**
     * Creates a new connected channel with the specified
     * <tt>"remoteAddress"</tt> and the current <tt>"localAddress"</tt> option.
     * If the <tt>"localAddress"</tt> option is not set, the local address of
     * a new channel is determined automatically.  This method is identical
     * with the following code:
     *
     * <pre>
     * {@link ConnectionlessBootstrap} b = ...;
     * b.connect(remoteAddress, b.getOption("localAddress"));
     * </pre>
     *
     * @return a future object which notifies when the creation of the connected
     *         channel succeeds or fails
     *
     * @throws ClassCastException
     *         if <tt>"localAddress"</tt> option's value is
     *            neither a {@link SocketAddress} nor <tt>null</tt>
     * @throws ChannelPipelineException
     *         if this bootstrap's {@link #setPipelineFactory(ChannelPipelineFactory) pipelineFactory}
     *            failed to create a new {@link ChannelPipeline}
     */
    ChannelFuturePtr connect(const SocketAddress& remoteAddress);

    /**
     * Creates a new connected channel with the specified
     * <tt>"remoteAddress"</tt> and the specified <tt>"localAddress"</tt>.
     * If the specified local address is <tt>null</tt>, the local address of a
     * new channel is determined automatically.
     *
     * @return a future object which notifies when the creation of the connected
     *         channel succeeds or fails
     *
     * @throws ChannelPipelineException
     *         if this bootstrap's {@link #setPipelineFactory(ChannelPipelineFactory) pipelineFactory}
     *            failed to create a new {@link ChannelPipeline}
     */
    ChannelFuturePtr connect(const SocketAddress& remoteAddress, const SocketAddress& localAddress);
};

}}

#endif //#if !defined(CETTY_BOOTSTRAP_CONNECTIONLESSBOOTSTRAP_H)
