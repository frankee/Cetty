#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIODATAGRAMCHANNELCONFIG_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIODATAGRAMCHANNELCONFIG_H

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

#include "cetty/channel/socket/DatagramChannelConfig.h"

namespace cetty { namespace channel { namespace socket { namespace asio { 

/**
 * A {@link DatagramChannelConfig} for a NIO TCP/IP {@link DatagramChannel}.
 *
 * <h3>Available options</h3>
 *
 * In addition to the options provided by {@link ChannelConfig} and
 * {@link DatagramChannelConfig}, {@link NioDatagramChannelConfig} allows the
 * following options in the option map:
 *
 * <table border="1" cellspacing="0" cellpadding="6">
 * <tr>
 * <th>Name</th><th>Associated setter method</th>
 * </tr><tr>
 * <td><tt>"writeBufferHighWaterMark"</tt></td><td>{@link #setWriteBufferHighWaterMark(int)}</td>
 * </tr><tr>
 * <td><tt>"writeBufferLowWaterMark"</tt></td><td>{@link #setWriteBufferLowWaterMark(int)}</td>
 * </tr><tr>
 * <td><tt>"writeSpinCount"</tt></td><td>{@link #setWriteSpinCount(int)}</td>
 * </tr><tr>
 * </table>
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author Daniel Bevenius (dbevenius@jboss.com)
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class AsioDatagramChannelConfig : public cetty::channel::socket::DatagramChannelConfig {
public:
    virtual ~AsioDatagramChannelConfig() {}

    /**
     * Gets the <a><tt>SO_SNDLOWAT</tt></a> option.
     */
    virtual int getWriteBufferLowWaterMark() const = 0;

    /**
     * Sets the <a><tt>SO_SNDLOWAT</tt></a> option.
     */
    virtual void setWriteBufferLowWaterMark(int writeBufferLowWaterMark) = 0;
    
    /**
     * Gets the <a><tt>SO_RCVLOWAT</tt></a> option.
     */
    virtual int getReceiveBufferLowWaterMark() const = 0;

    /**
     * Sets the <a><tt>SO_RCVLOWAT</tt></a> option.
     */
    virtual void setReceiveBufferLowWaterMark(int receiveBufferLowWaterMark) = 0;
};

}}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIODATAGRAMCHANNELCONFIG_H)
