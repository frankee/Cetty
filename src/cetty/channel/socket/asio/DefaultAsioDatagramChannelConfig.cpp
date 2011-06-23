/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.  
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
#include <boost/system/system_error.hpp>

#include "cetty/channel/ChannelException.h"
#include "cetty/channel/ReceiveBufferSizePredictor.h"
#include "cetty/channel/ReceiveBufferSizePredictorFactory.h"
#include "cetty/channel/FixedReceiveBufferSizePredictorFactory.h"
#include "cetty/channel/socket/asio/DefaultAsioDatagramChannelConfig.h"

#include "cetty/util/Exception.h"
#include "cetty/util/internal/ConversionUtil.h"

#include "cetty/logging/InternalLoggerFactory.h"

namespace cetty { namespace channel { namespace socket { namespace asio {

using namespace cetty::channel;
using namespace cetty::util;
using namespace cetty::util::internal;
using namespace cetty::logging;    

ReceiveBufferSizePredictorFactory* DefaultAsioDatagramChannelConfig::DEFAULT_PREDICTOR_FACTORY =
    new FixedReceiveBufferSizePredictorFactory(768);

InternalLogger *DefaultAsioDatagramChannelConfig::logger =
    InternalLoggerFactory::getInstance("DefaultNioDatagramChannelConfig");

DefaultAsioDatagramChannelConfig::DefaultAsioDatagramChannelConfig(udp_socket_type& socket)
    : socket(socket), predictor(NULL), predictorFactory(DEFAULT_PREDICTOR_FACTORY) {
    setChannelOwnBufferSize(DEFAULT_CHANNEL_OWN_BUFFER_SIZE);
}

bool DefaultAsioDatagramChannelConfig::setOption(const std::string& key, const boost::any& value) {
    if (DefaultDatagramChannelConfig::setOption(key, value)) {
        return true;
    }

    if (key == "writeBufferLowWaterMark") {
        setWriteBufferLowWaterMark(ConversionUtil::toInt(value));
    }
    else if (key == "receiveBufferLowWaterMark") {
        setReceiveBufferLowWaterMark(ConversionUtil::toInt(value));
    }
    else if (key == "broadcast") {
        setBroadcast(ConversionUtil::toBoolean(value));
    }
    else if (key == "receiveBufferSize") {
        setReceiveBufferSize(ConversionUtil::toInt(value));
    }
    else if (key == "sendBufferSize") {
        setSendBufferSize(ConversionUtil::toInt(value));
    }
    else if (key == "receiveBufferSizePredictorFactory") {
        ReceiveBufferSizePredictorFactory* const* factory =
            boost::any_cast<ReceiveBufferSizePredictorFactory*>(&value);
        if (factory && *factory) {
            setReceiveBufferSizePredictorFactory(*factory);
        }
    }
    else if (key == "receiveBufferSizePredictor") {
        ReceiveBufferSizePredictor* const* predictor =
            boost::any_cast<ReceiveBufferSizePredictor*>(&value);
        if (predictor && *predictor) {
            setReceiveBufferSizePredictor(*predictor);
        }
    }
    else if (key == "reuseAddress") {
        setReuseAddress(ConversionUtil::toBoolean(value));
    }
    else if (key == "loopbackModeDisabled") {
        setLoopbackModeDisabled(ConversionUtil::toBoolean(value));
    }
    else if (key == "interface") {
        const NetworkInterface* interface = boost::any_cast<NetworkInterface>(&value);
        if (interface) {
            setInterface(*interface);
        }
    }
    else if (key == "timeToLive") {
        setTimeToLive(ConversionUtil::toInt(value));
    }
    else {
        return false;
    }

    return true;
}

int DefaultAsioDatagramChannelConfig::getWriteBufferLowWaterMark() const {
    try {
        boost::asio::ip::udp::socket::send_low_watermark option;
        this->socket.get_option(option);
        return option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioDatagramChannelConfig::setWriteBufferLowWaterMark(int writeBufferLowWaterMark) {
    try {
        boost::asio::ip::udp::socket::send_low_watermark option(writeBufferLowWaterMark);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

int DefaultAsioDatagramChannelConfig::getReceiveBufferLowWaterMark() const {
    try {
        boost::asio::ip::udp::socket::receive_low_watermark option;
        this->socket.get_option(option);
        return option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioDatagramChannelConfig::setReceiveBufferLowWaterMark(int receiveBufferLowWaterMark) {
    try {
        boost::asio::ip::udp::socket::receive_low_watermark option(receiveBufferLowWaterMark);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

bool DefaultAsioDatagramChannelConfig::isBroadcast() const {
    try {
        boost::asio::ip::udp::socket::broadcast option;
        this->socket.get_option(option);
        return option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioDatagramChannelConfig::setBroadcast(bool broadcast) {
    try {
        boost::asio::ip::udp::socket::broadcast option(broadcast);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

cetty::channel::NetworkInterface DefaultAsioDatagramChannelConfig::getInterface() const {
    return outboundInterface;
}

void DefaultAsioDatagramChannelConfig::setInterface(const NetworkInterface& networkInterface) {
    try {
        if (networkInterface.isIPv6()) {
            boost::asio::ip::multicast::outbound_interface option(
                networkInterface.getIndex());

            this->socket.set_option(option);
        }
        else {
            boost::asio::ip::address_v4 address =
                boost::asio::ip::address_v4::from_string(
                    networkInterface.getIpAddress().toString());

            boost::asio::ip::multicast::outbound_interface option(address);
            this->socket.set_option(option);
        }

        outboundInterface = networkInterface;
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

bool DefaultAsioDatagramChannelConfig::isLoopbackModeDisabled() const {
    try {
        boost::asio::ip::multicast::enable_loopback option;
        this->socket.get_option(option);
        return !option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioDatagramChannelConfig::setLoopbackModeDisabled(bool loopbackModeDisabled) {
    try {
        boost::asio::ip::multicast::enable_loopback option(!loopbackModeDisabled);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

bool DefaultAsioDatagramChannelConfig::isReuseAddress() const {
    try {
        boost::asio::ip::udp::socket::broadcast option;
        this->socket.get_option(option);
        return option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(),e.code().value());
    }
}

void DefaultAsioDatagramChannelConfig::setReuseAddress(bool reuseAddress) {
    try {
        boost::asio::ip::udp::socket::reuse_address option(reuseAddress);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

int DefaultAsioDatagramChannelConfig::getReceiveBufferSize() const {
    try {
        boost::asio::ip::udp::socket::receive_buffer_size option;
        this->socket.get_option(option);
        return option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioDatagramChannelConfig::setReceiveBufferSize(int receiveBufferSize) {
    try {
        boost::asio::ip::udp::socket::receive_buffer_size option(receiveBufferSize);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

int DefaultAsioDatagramChannelConfig::getSendBufferSize() const {
    try {
        boost::asio::ip::udp::socket::send_buffer_size option;
        this->socket.get_option(option);
        return option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioDatagramChannelConfig::setSendBufferSize(int sendBufferSize) {
    try {
        boost::asio::ip::udp::socket::send_buffer_size option(sendBufferSize);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

int DefaultAsioDatagramChannelConfig::getTimeToLive() const {
    try {
        boost::asio::ip::multicast::hops option;
        this->socket.get_option(option);
        return !option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioDatagramChannelConfig::setTimeToLive(int ttl) {
    try {
        boost::asio::ip::multicast::hops option(ttl);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

ReceiveBufferSizePredictor* DefaultAsioDatagramChannelConfig::getReceiveBufferSizePredictor() {
    ReceiveBufferSizePredictor* predictor = this->predictor;
    if (predictor == NULL) {
        try {
            predictor = getReceiveBufferSizePredictorFactory()->getPredictor();
            this->predictor = predictor;
        }
        catch (const Exception& e) {
            throw ChannelException(
                "Failed to create a new ReceiveBufferSizePredictor", e);
        }
    }

    return predictor;
}

void DefaultAsioDatagramChannelConfig::setReceiveBufferSizePredictor(ReceiveBufferSizePredictor* predictor) {
    if (predictor == NULL) {
        throw NullPointerException("predictor");
    }

    this->predictor = predictor;
}

ReceiveBufferSizePredictorFactory* DefaultAsioDatagramChannelConfig::getReceiveBufferSizePredictorFactory() {
    return this->predictorFactory;
}

void DefaultAsioDatagramChannelConfig::setReceiveBufferSizePredictorFactory(ReceiveBufferSizePredictorFactory* predictorFactory) {
    if (predictorFactory == NULL) {
        throw NullPointerException("predictorFactory");
    }

    this->predictorFactory = predictorFactory;
}

}}}}