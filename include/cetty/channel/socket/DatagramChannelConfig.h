#if !defined(CETTY_CHANNEL_SOCKET_DATAGRAMCHANNELCONFIG_H)
#define CETTY_CHANNEL_SOCKET_DATAGRAMCHANNELCONFIG_H

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

#include "cetty/channel/NetworkInterface.h"

namespace cetty { namespace channel {
class ReceiveBufferSizePredictor;
class ReceiveBufferSizePredictorFactory;
}}

namespace cetty { namespace channel  { namespace socket {

using namespace cetty::channel;

/**
 * A {@link ChannelConfig} for a {@link DatagramChannel}.
 *
 * <h3>Available options</h3>
 *
 * In addition to the options provided by {@link ChannelConfig},
 * {@link DatagramChannelConfig} allows the following options in the option map:
 *
 * <table border="1" cellspacing="0" cellpadding="6">
 * <tr>
 * <th>Name</th><th>Associated setter method</th>
 * </tr><tr>
 * <td><tt>"broadcast"</tt></td><td>{@link #setBroadcast(bool)}</td>
 * </tr><tr>
 * <td><tt>"interface"</tt></td><td>{@link #setInterface(InetAddress)}</td>
 * </tr><tr>
 * <td><tt>"loopbackModeDisabled"</tt></td><td>{@link #setLoopbackModeDisabled(bool)}</td>
 * </tr><tr>
 * <td><tt>"networkInterface"</tt></td><td>{@link #setNetworkInterface(NetworkInterface)}</td>
 * </tr><tr>
 * <td><tt>"reuseAddress"</tt></td><td>{@link #setReuseAddress(bool)}</td>
 * </tr><tr>
 * <td><tt>"receiveBufferSize"</tt></td><td>{@link #setReceiveBufferSize(int)}</td>
 * </tr><tr>
 * <td><tt>"receiveBufferSizePredictor"</tt></td><td>{@link #setReceiveBufferSizePredictor(ReceiveBufferSizePredictor)}</td>
 * </tr><tr>
 * <td><tt>"receiveBufferSizePredictorFactory"</tt></td><td>{@link #setReceiveBufferSizePredictorFactory(ReceiveBufferSizePredictorFactory)}</td>
 * </tr><tr>
 * <td><tt>"sendBufferSize"</tt></td><td>{@link #setSendBufferSize(int)}</td>
 * </tr><tr>
 * <td><tt>"timeToLive"</tt></td><td>{@link #setTimeToLive(int)}</td>
 * </tr><tr>
 * <td><tt>"trafficClass"</tt></td><td>{@link #setTrafficClass(int)}</td>
 * </tr>
 * </table>
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */

class DatagramChannelConfig {
public:
	virtual ~DatagramChannelConfig() {}

    /**
     * Gets the <a><tt>SO_SNDBUF</tt></a> option.
     */
    virtual int getSendBufferSize() const = 0;

    /**
     * Sets the <a><tt>SO_SNDBUF</tt></a> option.
     */
    virtual void setSendBufferSize(int sendBufferSize) = 0;

    /**
     * Gets the <a><tt>SO_RCVBUF</tt></a> option.
     */
    virtual int getReceiveBufferSize() const = 0;

    /**
     * Gets the <a><tt>SO_RCVBUF</tt></a> option.
     */
    virtual void setReceiveBufferSize(int receiveBufferSize) = 0;

    /**
     * Gets the <a><tt>SO_REUSEADDR</tt></a> option.
     */
    virtual bool isReuseAddress() const = 0;

    /**
     * Sets the <a><tt>SO_REUSEADDR</tt></a> option.
     */
    virtual void setReuseAddress(bool reuseAddress) = 0;

    /**
     * Gets the <a><tt>SO_BROADCAST</tt></a> option.
     */
    virtual bool isBroadcast() const = 0;

    /**
     * Sets the <a><tt>SO_BROADCAST</tt></a> option.
     */
    virtual void setBroadcast(bool broadcast) = 0;

    /**
     * Gets the setting for local loopback of multicast datagrams.
     *
     * @return <tt>true</tt> if and only if the loopback mode has been disabled
     */
    virtual bool isLoopbackModeDisabled() const = 0;

    /**
     * Sets the setting for local loopback of multicast datagrams.
     *
     * @param loopbackModeDisabled
     *        <tt>true</tt> if and only if the loopback mode has been disabled
     */
    virtual void setLoopbackModeDisabled(bool loopbackModeDisabled) = 0;

    /**
     * Gets the default time-to-live for multicast packets sent out on the
     * socket.
     */
    virtual int getTimeToLive() const = 0;

    /**
     * Sets the default time-to-live for multicast packets sent out on the
     * {@link DatagramChannel} in order to control the scope of the multicasts.
     */
    virtual void setTimeToLive(int ttl) = 0;

    /**
     * Gets the address of the network interface used for multicast packets.
     */
    virtual NetworkInterface getInterface() const = 0;

    /**
     * Sets the address of the network interface used for multicast packets.
     */
    virtual void setInterface(const NetworkInterface& networkInterface) = 0;

    /**
     * Returns the {@link ReceiveBufferSizePredictor} which predicts the
     * number of readable bytes in the socket receive buffer.  The default
     * predictor is <tt>{@link FixedReceiveBufferSizePredictor}(768)</tt>.
     */
    virtual ReceiveBufferSizePredictor* getReceiveBufferSizePredictor() = 0;

    /**
     * Sets the {@link ReceiveBufferSizePredictor} which predicts the
     * number of readable bytes in the socket receive buffer.  The default
     * predictor is <tt>{@link FixedReceiveBufferSizePredictor}(768)</tt>.
     */
    virtual void setReceiveBufferSizePredictor(ReceiveBufferSizePredictor* predictor) = 0;

    /**
     * Returns the {@link ReceiveBufferSizePredictorFactory} which creates a new
     * {@link ReceiveBufferSizePredictor} when a new channel is created and
     * no {@link ReceiveBufferSizePredictor} was set.  If no predictor was set
     * for the channel, {@link #setReceiveBufferSizePredictor(ReceiveBufferSizePredictor)}
     * will be called with the new predictor.  The default factory is
     * <tt>{@link FixedReceiveBufferSizePredictorFactory}(768)</tt>.
     */
    virtual ReceiveBufferSizePredictorFactory* getReceiveBufferSizePredictorFactory() = 0;

    /**
     * Sets the {@link ReceiveBufferSizePredictor} which creates a new
     * {@link ReceiveBufferSizePredictor} when a new channel is created and
     * no {@link ReceiveBufferSizePredictor} was set.  If no predictor was set
     * for the channel, {@link #setReceiveBufferSizePredictor(ReceiveBufferSizePredictor)}
     * will be called with the new predictor.  The default factory is
     * <tt>{@link FixedReceiveBufferSizePredictorFactory}(768)</tt>.
     */
    virtual void setReceiveBufferSizePredictorFactory(ReceiveBufferSizePredictorFactory* predictorFactory) = 0;
};

}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_DATAGRAMCHANNELCONFIG_H)
