#if !defined(CETTY_CHANNEL_SOCKET_ASIO_DEFAULTASIODATAGRAMCHANNELCONFIG_H)
#define CETTY_CHANNEL_SOCKET_ASIO_DEFAULTASIODATAGRAMCHANNELCONFIG_H

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

#include <string>
#include <boost/any.hpp>
#include <boost/asio.hpp>

#include "cetty/channel/IpAddress.h"
#include "cetty/channel/NetworkInterface.h"
#include "cetty/channel/ChannelException.h"
#include "cetty/channel/socket/DefaultDatagramChannelConfig.h"
#include "cetty/channel/socket/asio/AsioDatagramChannelConfig.h"

namespace cetty { namespace logging {
    class InternalLogger;
}}

namespace cetty { namespace channel {
class ReceiveBufferSizePredictor;
class ReceiveBufferSizePredictorFactory;
}}

namespace cetty { namespace channel { namespace socket { namespace asio {

using namespace cetty::channel;
using namespace cetty::logging;

/**
 * The default {@link AsioSocketChannelConfig} implementation.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author Daniel Bevenius (dbevenius@jboss.com)
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 *
 */
class DefaultAsioDatagramChannelConfig
    : public cetty::channel::socket::DefaultDatagramChannelConfig,
      public AsioDatagramChannelConfig {

public:
    typedef boost::asio::ip::udp::socket udp_socket_type;

public:
    DefaultAsioDatagramChannelConfig(udp_socket_type& socket);

    virtual bool setOption(const std::string& key, const boost::any& value);

    virtual int getWriteBufferLowWaterMark() const;
    virtual void setWriteBufferLowWaterMark(int writeBufferLowWaterMark);

    virtual int getReceiveBufferLowWaterMark() const;
    virtual void setReceiveBufferLowWaterMark(int receiveBufferLowWaterMark);

    virtual bool isBroadcast() const;
    virtual void setBroadcast(bool broadcast);

    virtual NetworkInterface getInterface() const;
    virtual void setInterface(const NetworkInterface& networkInterface);

    virtual bool isLoopbackModeDisabled() const;
    virtual void setLoopbackModeDisabled(bool loopbackModeDisabled);

    virtual bool isReuseAddress() const;
    virtual void setReuseAddress(bool reuseAddress);

    virtual int getReceiveBufferSize() const;
    virtual void setReceiveBufferSize(int receiveBufferSize);

    virtual int getSendBufferSize() const;
    virtual void setSendBufferSize(int sendBufferSize);

    virtual int getTimeToLive() const;
    virtual void setTimeToLive(int ttl);

    virtual ReceiveBufferSizePredictor* getReceiveBufferSizePredictor();
    virtual void setReceiveBufferSizePredictor(ReceiveBufferSizePredictor* predictor);

    virtual ReceiveBufferSizePredictorFactory* getReceiveBufferSizePredictorFactory();
    virtual void setReceiveBufferSizePredictorFactory(ReceiveBufferSizePredictorFactory* predictorFactory);

    virtual bool channelOwnBuffer() const { return false; }

    void joinGroup(const IpAddress& multicastAddress) {
        joinGroup(multicastAddress, NetworkInterface());
    }
    void joinGroup(const IpAddress& multicastAddress,
                   const NetworkInterface& networkInterface) {
        using boost::asio::ip::multicast::join_group;
        joinOrLeaveGroup<join_group>(multicastAddress, networkInterface);
    }

    void leaveGroup(const IpAddress& multicastAddress) {
        leaveGroup(multicastAddress, NetworkInterface());
    }
    
    void leaveGroup(const IpAddress& multicastAddress,
                    const NetworkInterface& networkInterface) {
        using boost::asio::ip::multicast::leave_group;
        joinOrLeaveGroup<leave_group>(multicastAddress, networkInterface);
    }

private:

    template<typename OptionT>
    void joinOrLeaveGroup(const IpAddress& multicastAddress, const NetworkInterface& networkInterface) {
        try {
            using namespace boost::asio::ip;
            address addr = address::from_string(multicastAddress.toString());

            if (addr.is_v6()) {
                if (networkInterface == NetworkInterface::NULL_INTERFACE) {
                    OptionT option(addr);
                    this->socket.set_option(option);
                }
                else {
                    OptionT option(addr.to_v6(), networkInterface.getIndex());
                    this->socket.set_option(option);
                }
            }
            else {
                if (networkInterface == NetworkInterface::NULL_INTERFACE) {
                    OptionT option(addr);
                    this->socket.set_option(option);
                }
                else {
                    address_v4 interfaceAddress = address_v4::from_string(
                        networkInterface.getIpAddress().toString());
                    OptionT option(addr.to_v4(), interfaceAddress);
                    this->socket.set_option(option);
                }
            }
        }
        catch (const Exception& e) {
            throw ChannelException(e.what());
        }
    }

private:
    static const int DEFAULT_CHANNEL_OWN_BUFFER_SIZE = 1024 * 1024 * 4;

private:
    static InternalLogger *logger;
    static ReceiveBufferSizePredictorFactory* DEFAULT_PREDICTOR_FACTORY;
    
    udp_socket_type& socket;

    NetworkInterface outboundInterface;
    ReceiveBufferSizePredictor* predictor;
    ReceiveBufferSizePredictorFactory* predictorFactory;
};

}}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_DEFAULTASIODATAGRAMCHANNELCONFIG_H)
