#if !defined(CETTY_CHANNEL_SOCKET_DATAGRAMCHANNEL_H)
#define CETTY_CHANNEL_SOCKET_DATAGRAMCHANNEL_H

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

#include "cetty/channel/AbstractChannel.h"

namespace cetty { namespace channel {

class IpAddress;
class SocketAddress;
class NetworkInterface;

}}

namespace cetty { namespace channel  { namespace socket {

/**
 * A UDP/IP {@link Channel} which is created by {@link DatagramChannelFactory}.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 * @apiviz.composedOf org.jboss.netty.channel.socket.DatagramChannelConfig
 */

class DatagramChannel  : public ::cetty::channel::AbstractChannel {
public:
    /**
     * Joins a multicast group.
     */
    virtual void joinGroup(const IpAddress& multicastAddress) = 0;

    /**
     * Joins the specified multicast group at the specified interface.
     */
    virtual void joinGroup(const IpAddress& multicastAddress, const NetworkInterface& networkInterface) = 0;

    /**
     * Leaves a multicast group.
     */
    virtual void leaveGroup(const IpAddress& multicastAddress) = 0;

    /**
     * Leaves a multicast group on a specified local interface.
     */
    virtual void leaveGroup(const IpAddress& multicastAddress, const NetworkInterface& networkInterface) = 0;

protected:
    DatagramChannel(
        Channel* parent, ChannelFactory* factory,
        ChannelPipeline* pipeline, ChannelSink* sink)
        : AbstractChannel(parent, factory, pipeline, sink) {}

    virtual ~DatagramChannel() {}
};

}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_DATAGRAMCHANNEL_H)
