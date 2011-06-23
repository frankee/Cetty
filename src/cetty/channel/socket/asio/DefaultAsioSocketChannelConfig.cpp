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

#include <typeinfo>

#include "cetty/channel/ReceiveBufferSizePredictor.h"
#include "cetty/channel/ReceiveBufferSizePredictorFactory.h"
#include "cetty/channel/AdaptiveReceiveBufferSizePredictorFactory.h"
#include "cetty/channel/ChannelException.h"
#include "cetty/channel/socket/asio/DefaultAsioSocketChannelConfig.h"

#include "cetty/util/internal/ConversionUtil.h"
#include "cetty/util/Exception.h"

#include "cetty/logging/InternalLoggerFactory.h"


namespace cetty { namespace channel { namespace socket { namespace asio {

using namespace ::cetty::channel;
using namespace ::cetty::util;
using namespace ::cetty::util::internal;
using namespace ::cetty::logging;

InternalLogger* DefaultAsioSocketChannelConfig::logger =
                    InternalLoggerFactory::getInstance("DefaultAsioSocketChannelConfig");

static AdaptiveReceiveBufferSizePredictorFactory adaptiveFactory;

ReceiveBufferSizePredictorFactory* 
DefaultAsioSocketChannelConfig::DEFAULT_PREDICTOR_FACTORY= &adaptiveFactory;

bool DefaultAsioSocketChannelConfig::setOption(const std::string& key, const boost::any& value) {
    if (DefaultSocketChannelConfig::setOption(key, value)) {
        return true;
    }

    if (key == "receiveBufferSize") {
        setReceiveBufferSize(ConversionUtil::toInt(value));
    }
    else if (key == "sendBufferSize") {
        setSendBufferSize(ConversionUtil::toInt(value));
    }
    else if (key == "tcpNoDelay") {
        setTcpNoDelay(ConversionUtil::toBoolean(value));
    }
    else if (key == "keepAlive") {
        setKeepAlive(ConversionUtil::toBoolean(value));
    }
    else if (key == "reuseAddress") {
        setReuseAddress(ConversionUtil::toBoolean(value));
    }
    else if (key == "soLinger") {
        setSoLinger(ConversionUtil::toInt(value));
    }
    else if (key == "writeBufferLowWaterMark") {
        setWriteBufferLowWaterMark(ConversionUtil::toInt(value));
    }
    else if (key == "receiveBufferLowWaterMark") {
        setReceiveBufferLowWaterMark(ConversionUtil::toInt(value));
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
    else {
        return false;
    }

    return true;
}

int DefaultAsioSocketChannelConfig::getReceiveBufferSize() const {
    try {
        boost::asio::ip::tcp::socket::receive_buffer_size option;
        this->socket.get_option(option);
        return option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

int DefaultAsioSocketChannelConfig::getSendBufferSize() const {
    try {
        boost::asio::ip::tcp::socket::send_buffer_size option;
        this->socket.get_option(option);
        return option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

int DefaultAsioSocketChannelConfig::getSoLinger() const {
    try {
        boost::asio::ip::tcp::socket::linger option;
        this->socket.get_option(option);
        return option.enabled() ? 0 : option.timeout();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

bool DefaultAsioSocketChannelConfig::isKeepAlive() const {
    try {
        boost::asio::ip::tcp::socket::keep_alive option;
        this->socket.get_option(option);
        return option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

bool DefaultAsioSocketChannelConfig::isReuseAddress() const {
    try {
        boost::asio::ip::tcp::socket::reuse_address option;
        this->socket.get_option(option);
        return option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

bool DefaultAsioSocketChannelConfig::isTcpNoDelay() const {
    try {
        boost::asio::ip::tcp::no_delay option;
        this->socket.get_option(option);
        return option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioSocketChannelConfig::setKeepAlive(bool keepAlive) {
    try {
        boost::asio::ip::tcp::socket::keep_alive option(keepAlive);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioSocketChannelConfig::setPerformancePreferences(int connectionTime, int latency, int bandwidth) {
}

void DefaultAsioSocketChannelConfig::setReceiveBufferSize(int receiveBufferSize) {
    try {
        boost::asio::ip::tcp::socket::receive_buffer_size option(receiveBufferSize);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioSocketChannelConfig::setReuseAddress(bool reuseAddress) {
    try {
        boost::asio::ip::tcp::socket::reuse_address option(reuseAddress);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioSocketChannelConfig::setSendBufferSize(int sendBufferSize) {
    try {
        boost::asio::ip::tcp::socket::send_buffer_size option(sendBufferSize);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioSocketChannelConfig::setSoLinger(int soLinger) {
    try {
        boost::asio::ip::tcp::socket::linger option;
        if (soLinger > 0) {
            option.enabled(true);
            option.timeout(soLinger);
        }
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioSocketChannelConfig::setTcpNoDelay(bool tcpNoDelay) {
    try {
        boost::asio::ip::tcp::no_delay option(tcpNoDelay);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioSocketChannelConfig::setWriteBufferLowWaterMark(int writeBufferLowWaterMark) {
    try {
        boost::asio::ip::tcp::socket::send_low_watermark option(writeBufferLowWaterMark);
        this->socket.set_option(option);

        this->writeBufferLowWaterMark = writeBufferLowWaterMark;
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

int DefaultAsioSocketChannelConfig::getWriteBufferLowWaterMark() const {
    if (writeBufferLowWaterMark == 0) {
        try {
            boost::asio::ip::tcp::socket::send_low_watermark option;
            this->socket.get_option(option);
            writeBufferLowWaterMark = option.value();
        }
        catch (const boost::system::system_error& e) {            
            //it will throw an exception in windows XP.
            //however, we do not care the exception, just set default value.
            writeBufferLowWaterMark = DEFAULT_WRITE_BUFFER_LOW_WATERMARK;
            //throw ChannelException(e.what(), e.code().value());
        }
    }

    return writeBufferLowWaterMark;
}

void DefaultAsioSocketChannelConfig::setReceiveBufferLowWaterMark(int receiveBufferLowWaterMark) {
    try {
        boost::asio::ip::tcp::socket::receive_low_watermark option(receiveBufferLowWaterMark);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

int DefaultAsioSocketChannelConfig::getReceiveBufferLowWaterMark() const {
    try {
        boost::asio::ip::tcp::socket::receive_low_watermark option;
        this->socket.get_option(option);
        return option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

ReceiveBufferSizePredictor* DefaultAsioSocketChannelConfig::getReceiveBufferSizePredictor() {
    ReceiveBufferSizePredictor* predictor = this->predictor;
    if (predictor != NULL) return predictor;
    
    try {
        predictor = getReceiveBufferSizePredictorFactory()->getPredictor();
        this->predictor = predictor;
    }
    catch (const Exception& e) {
        throw ChannelException(
            std::string("Failed to create a new ") + typeid(*this).name(), e.getCode());
    }

    return predictor;
}

void DefaultAsioSocketChannelConfig::setReceiveBufferSizePredictor(ReceiveBufferSizePredictor* predictor) {
    if (predictor == NULL) {
        throw NullPointerException("predictor");
    }
    this->predictor = predictor;
}

void DefaultAsioSocketChannelConfig::setReceiveBufferSizePredictorFactory(ReceiveBufferSizePredictorFactory* predictorFactory) {
    if (predictorFactory == NULL) {
        throw NullPointerException("predictorFactory");
    }
    this->predictorFactory = predictorFactory;
}

}}}}