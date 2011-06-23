#if !defined(CETTY_CHANNEL_SOCKET_ASIO_DEFAULTASIOSERVERSOCKETCHANNELCONFIG_H)
#define CETTY_CHANNEL_SOCKET_ASIO_DEFAULTASIOSERVERSOCKETCHANNELCONFIG_H

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

#include <boost/any.hpp>
#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>

#include "cetty/channel/ChannelException.h"
#include "cetty/channel/socket/DefaultServerSocketChannelConfig.h"

#include "cetty/util/Exception.h"
#include "cetty/util/internal/ConversionUtil.h"

namespace cetty { namespace channel { namespace socket { namespace asio {

using namespace cetty::channel;
using namespace cetty::channel::socket;
using namespace cetty::util;

class DefaultAsioServerSocketChannelConfig
    : public cetty::channel::socket::DefaultServerSocketChannelConfig {

public:
    /**
     * Creates a new instance.
     */
    DefaultAsioServerSocketChannelConfig(boost::asio::ip::tcp::acceptor& acceptor)
        : acceptor(acceptor) {
    }

    virtual bool setOption(const std::string& key, const boost::any& value) {
        if (DefaultServerChannelConfig::setOption(key, value)) {
            return true;
        }

        if (key.compare("receiveBufferSize") == 0) {
            setReceiveBufferSize(internal::ConversionUtil::toInt(value));
        }
        else if (key.compare("reuseAddress") == 0) {
            setReuseAddress(internal::ConversionUtil::toBoolean(value));
        }
        else if (key.compare("backlog") == 0) {
            setBacklog(internal::ConversionUtil::toInt(value));
        }
        else {
            return false;
        }
        return true;
    }

    virtual bool isReuseAddress() const {
        try {
            boost::asio::ip::tcp::acceptor::reuse_address option;
            acceptor.get_option(option);
            return option.value();
        }
        catch (const boost::system::system_error& e) {
            throw ChannelException(e.what(), e.code().value());
        }
    }

    virtual void setReuseAddress(bool reuseAddress) {
        try {
            boost::asio::ip::tcp::acceptor::reuse_address option(reuseAddress);
            acceptor.set_option(option);
        }
        catch (const boost::system::system_error& e) {
            throw ChannelException(e.what(), e.code().value());
        }
    }

    virtual int getReceiveBufferSize() const {
        try {
            boost::asio::ip::tcp::acceptor::receive_buffer_size option;
            acceptor.get_option(option);
            return option.value();
        }
        catch (const boost::system::system_error& e) {
            throw ChannelException(e.what(), e.code().value());
        }
    }

    virtual void setReceiveBufferSize(int receiveBufferSize) {
        try {
            boost::asio::ip::tcp::acceptor::receive_buffer_size option(receiveBufferSize);
            acceptor.set_option(option);
        }
        catch (const boost::system::system_error& e) {
            throw ChannelException(e.what(), e.code().value());
        }
    }

    virtual void setPerformancePreferences(int connectionTime, int latency, int bandwidth) {
        //socket.setPerformancePreferences(connectionTime, latency, bandwidth);
    }

    virtual int getBacklog() const {
        return this->backlog;
    }

    virtual void setBacklog(int backlog) {
        if (backlog < 0) {
            InvalidArgumentException("backlog: is less then zero");
        }
        this->backlog = backlog;
    }

    virtual bool channelOwnBuffer() const { return false; }

private:
    boost::asio::ip::tcp::acceptor& acceptor;
    int backlog;
};

}}}}


#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_DEFAULTASIOSERVERSOCKETCHANNELCONFIG_H)
