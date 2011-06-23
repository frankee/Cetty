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

#include "cetty/channel/socket/asio/AsioDatagramChannel.h"

#include <boost/bind.hpp>

#include "cetty/buffer/ChannelBuffer.h"
#include "cetty/buffer/ChannelBufferFactory.h"

#include "cetty/channel/Channels.h"
#include "cetty/channel/ChannelConfig.h"
#include "cetty/channel/ChannelFactory.h"
#include "cetty/channel/ChannelPipeline.h"
#include "cetty/channel/ChannelSink.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/DefaultWriteCompletionEvent.h"

#include "cetty/channel/socket/asio/AsioIpAddressImpl.h"
#include "cetty/channel/socket/asio/AsioSocketAddressImpl.h"
#include "cetty/channel/socket/asio/AsioDatagramChannelConfig.h"
#include "cetty/channel/socket/asio/DefaultAsioDatagramChannelConfig.h"

#include "cetty/util/Exception.h"

namespace cetty { namespace channel { namespace socket { namespace asio {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;

AsioDatagramChannel::AsioDatagramChannel(ChannelFactory* factory,
                                         ChannelPipeline* pipeline,
                                         ChannelSink* sink,
                                         AsioServicePool::IOService& ioService,
                                         int ioThreadCount,
                                         int ipProtocol)
    : DatagramChannel(NULL, factory, pipeline, sink),
      ioThreadCount(ioThreadCount),
      ioService(ioService),
      udpSocket(ioService),
      config(udpSocket),
      remoteAddressImplPr(new AsioUdpSocketAddressImpl(ioService)){
    try {
      if (ipProtocol == IpAddress::IPv4) {
          udpSocket.open(boost::asio::ip::udp::v4());
      }
      else {
          udpSocket.open(boost::asio::ip::udp::v6());
      }
    }
    catch (const boost::system::system_error& e) {
      throw ChannelException("Failed to open a server socket.", e.code().value());
    }

    Channels::fireChannelOpen(*this);
    ChannelBufferFactory* bufferFactory = config.getBufferFactory();
    readBuffer = bufferFactory->getBuffer(bufferFactory->getDefaultOrder(),
                                          config.getChannelOwnBufferSize());
}

const SocketAddress& AsioDatagramChannel::getLocalAddress() const {
    if (localAddress != SocketAddress::NULL_ADDRESS) {
        return localAddress;
    }

    boost::system::error_code ec;
    boost::asio::ip::udp::endpoint endpoint = udpSocket.local_endpoint(ec);

    if (ec) {
        return SocketAddress::NULL_ADDRESS;
    }

    localAddress = SocketAddress(
        SocketAddress::SmartPointer(new AsioUdpSocketAddressImpl(ioService, endpoint)));

    return localAddress;
}

const SocketAddress& AsioDatagramChannel::getRemoteAddress() const {
    if (remoteAddress != SocketAddress::NULL_ADDRESS) {
        return remoteAddress;
    }

    boost::system::error_code ec;
    boost::asio::ip::udp::endpoint endpoint = udpSocket.remote_endpoint(ec);

    if (ec) {
        return SocketAddress::NULL_ADDRESS;
    }

    remoteAddress = SocketAddress(
        SocketAddress::SmartPointer(new AsioUdpSocketAddressImpl(ioService, endpoint)));

    return remoteAddress;
}

ChannelConfig& AsioDatagramChannel::getConfig() {
    return this->config;
}

const ChannelConfig& AsioDatagramChannel::getConfig() const {
    return this->config;
}

int AsioDatagramChannel::getInterestOps() const {
    if (!isOpen()) {
        return Channel::OP_WRITE;
    }

    int interestOps = getRawInterestOps();
#if 0
    int writeBufferSize = this.writeBufferSize.get();
    if (writeBufferSize != 0) {
        if (highWaterMarkCounter.get() > 0) {
            int lowWaterMark = getConfig().getWriteBufferLowWaterMark();
            if (writeBufferSize >= lowWaterMark) {
                interestOps |= Channel.OP_WRITE;
            }
            else {
                interestOps &= ~Channel.OP_WRITE;
            }
        }
        else {
            int highWaterMark = getConfig().getWriteBufferHighWaterMark();
            if (writeBufferSize >= highWaterMark) {
                interestOps |= Channel.OP_WRITE;
            }
            else {
                interestOps &= ~Channel.OP_WRITE;
            }
        }
    }
    else {
        interestOps &= ~Channel.OP_WRITE;
    }
#endif
    return interestOps;
}

int AsioDatagramChannel::getRawInterestOps() const {
    return AbstractChannel::getInterestOps();
}

void AsioDatagramChannel::setRawInterestOpsNow(int interestOps) {
    AbstractChannel::setInterestOpsNow(interestOps);
}

ChannelFuturePtr AsioDatagramChannel::write(const ChannelMessage& message,
                                            const SocketAddress& remoteAddress) {
    if (remoteAddress == SocketAddress::NULL_ADDRESS
        || remoteAddress == getRemoteAddress()) {
            return DatagramChannel::write(message, SocketAddress::NULL_ADDRESS);
    }
    else {
        return DatagramChannel::write(message, remoteAddress);
    }
}

void AsioDatagramChannel::handleReceiveFrom(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error) {
        readBuffer->offsetWriterIndex(bytes_transferred);

        // Fire the event.
        Channels::fireMessageReceived(*this,
                                      ChannelMessage(readBuffer),
                                      SocketAddress(remoteAddressImplPr));

        AsioUdpSocketAddressImpl* addressImpl = static_cast<AsioUdpSocketAddressImpl*>(remoteAddressImplPr.get());
        Array arry;
        readBuffer->writableBytes(arry);

        udpSocket.async_receive_from(
            boost::asio::buffer(arry.data(), arry.length()),
            addressImpl->getEndpoint(),
            boost::bind(&AsioDatagramChannel::handleReceiveFrom,
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }
    else {
        close();
    }
}

void AsioDatagramChannel::handleSendTo(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error) {
        writeQueue.poll().setSuccess();

        pipeline->sendUpstream(DefaultWriteCompletionEvent(*this, bytes_transferred));
        //Channels::fireWriteComplete(*this, bytes_transferred);
    }
    else {
        writeQueue.poll().setFailure(
            RuntimeException(std::string("write buffer failed, code=") +
            Integer::toString(error.value())));
        close();
    }
}

void AsioDatagramChannel::joinGroup(const IpAddress& multicastAddress) {
    config.joinGroup(multicastAddress);
}

void AsioDatagramChannel::joinGroup(const IpAddress& multicastAddress, const NetworkInterface& networkInterface) {
    config.joinGroup(multicastAddress, networkInterface);
}

void AsioDatagramChannel::leaveGroup(const IpAddress& multicastAddress) {
    config.leaveGroup(multicastAddress);
}

void AsioDatagramChannel::leaveGroup(const IpAddress& multicastAddress, const NetworkInterface& networkInterface) {
    config.leaveGroup(multicastAddress, networkInterface);
}

bool AsioDatagramChannel::setClosed() {
    return AbstractChannel::setClosed();
}

boost::asio::ip::udp::endpoint& AsioDatagramChannel::getEndpoint() {
    AsioUdpSocketAddressImpl* addressImpl = static_cast<AsioUdpSocketAddressImpl*>(remoteAddressImplPr.get());
    return addressImpl->getEndpoint();
}

void AsioDatagramChannel::sendto(const MessageEvent& evt) {
    const ChannelFuturePtr& f = evt.getFuture();
    const SocketAddress& address = evt.getRemoteAddress();

    AsioWriteRequest writeRequest(evt);
    EndpointType endpoint(
        boost::asio::ip::address::from_string(address.address()),
        address.port());

    writeQueue.offer(writeRequest, f);

    if (writeRequest.writeBufferSize == 0) {
        ioService.service().post(boost::bind(
            &AsioDatagramChannel::handleSendTo, this, boost::system::error_code(), 0));
        return;
    }

    if (writeRequest.hasBuffers()) {
        udpSocket.async_send_to(
            writeRequest.gathring.buffers,
            endpoint,
            make_custom_alloc_handler(writeAllocator,
                boost::bind(&AsioDatagramChannel::handleSendTo,
                            this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred)));
    }
    else {
        udpSocket.async_send_to(
            boost::asio::const_buffers_1(writeRequest.buffer),
            endpoint,
            make_custom_alloc_handler(writeAllocator,
                boost::bind(&AsioDatagramChannel::handleSendTo,
                            this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred)));
    }
}

void AsioDatagramChannel::cleanUpWriteBuffer() {

}

}}}}
