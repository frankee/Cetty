#if !defined(CETTY_CHANNEL_SOCKET_ASIO_DEFAULTASIOSOCKETCHANNELCONFIG_H)
#define CETTY_CHANNEL_SOCKET_ASIO_DEFAULTASIOSOCKETCHANNELCONFIG_H

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
#include "cetty/channel/socket/DefaultSocketChannelConfig.h"
#include "cetty/channel/socket/asio/AsioSocketChannelConfig.h"


namespace cetty { namespace logging {
class InternalLogger;
}}

namespace cetty { namespace channel  { namespace socket { namespace asio {

using namespace cetty::channel;
using namespace cetty::logging;

class DefaultAsioSocketChannelConfig : public cetty::channel::socket::DefaultSocketChannelConfig {
public:
    typedef boost::asio::ip::tcp::socket tcp_socket_type;

public:
    DefaultAsioSocketChannelConfig(tcp_socket_type& socket)
        : DefaultSocketChannelConfig(),
          socket(socket),
          writeBufferLowWaterMark(0),
          writeBufferHighWaterMark(DEFAULT_WRITE_BUFFER_HIGH_WATERMARK),
          predictor(NULL),
          predictorFactory(DEFAULT_PREDICTOR_FACTORY) {
        setChannelOwnBufferSize(DEFAULT_CHANNEL_OWN_BUFFER_SIZE);
    }

    virtual bool setOption(const std::string& key, const boost::any& value);

    virtual int getReceiveBufferSize() const;
    virtual int getSendBufferSize() const;
    virtual int getSoLinger() const;

    virtual bool isKeepAlive() const;
    virtual bool isReuseAddress() const;
    virtual bool isTcpNoDelay() const;

    virtual void setKeepAlive(bool keepAlive);
    virtual void setPerformancePreferences(int connectionTime, int latency, int bandwidth);
    virtual void setReceiveBufferSize(int receiveBufferSize);
    virtual void setReuseAddress(bool reuseAddress);
    virtual void setSendBufferSize(int sendBufferSize);
    virtual void setSoLinger(int soLinger);
    virtual void setTcpNoDelay(bool tcpNoDelay);

    virtual int  getWriteBufferHighWaterMark() const {
        return writeBufferHighWaterMark;
    }
    virtual void setWriteBufferHighWaterMark(int writeBufferHighWaterMark) {
        this->writeBufferHighWaterMark = writeBufferHighWaterMark;
    }

    virtual int  getWriteBufferLowWaterMark() const;
    virtual void setWriteBufferLowWaterMark(int writeBufferLowWaterMark);

    virtual int  getReceiveBufferLowWaterMark() const;
    virtual void setReceiveBufferLowWaterMark(int receiveBufferLowWaterMark);

    virtual ReceiveBufferSizePredictor* getReceiveBufferSizePredictor();
    virtual void setReceiveBufferSizePredictor(ReceiveBufferSizePredictor* predictor);

    virtual ReceiveBufferSizePredictorFactory* getReceiveBufferSizePredictorFactory() const {
        return this->predictorFactory;
    }
    virtual void setReceiveBufferSizePredictorFactory(ReceiveBufferSizePredictorFactory* predictorFactory);

    virtual bool channelOwnBuffer() const { return true; }

private:
    static const int DEFAULT_CHANNEL_OWN_BUFFER_SIZE = 1024 * 32;
    static const int DEFAULT_WRITE_BUFFER_HIGH_WATERMARK = 2 * 1024 * 1024;
    static const int DEFAULT_WRITE_BUFFER_LOW_WATERMARK  = 2 * 1024;

private:
    static InternalLogger* logger;
    static ReceiveBufferSizePredictorFactory* DEFAULT_PREDICTOR_FACTORY;

    tcp_socket_type& socket;

    mutable int writeBufferLowWaterMark;
    int writeBufferHighWaterMark;

    ReceiveBufferSizePredictor* predictor;
    ReceiveBufferSizePredictorFactory* predictorFactory;
};

}}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_DEFAULTASIOSOCKETCHANNELCONFIG_H)
