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

#include "cetty/channel/socket/asio/AsioSocketChannel.h"
#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/ChannelException.h"
#include "cetty/channel/ChannelPipeline.h"
#include "cetty/channel/UpstreamMessageEvent.h"
#include "cetty/channel/DownstreamMessageEvent.h"
#include "cetty/channel/DownstreamChannelStateEvent.h"
#include "cetty/channel/CopyableDownstreamMessageEvent.h"
#include "cetty/channel/CopyableDownstreamChannelStateEvent.h"
#include "cetty/channel/DefaultWriteCompletionEvent.h"
#include "cetty/channel/socket/asio/AsioSocketAddressImpl.h"

#include "cetty/buffer/ChannelBuffer.h"
#include "cetty/buffer/ChannelBufferFactory.h"
#include "cetty/buffer/CompositeChannelBuffer.h"

#include "cetty/util/Integer.h"

namespace cetty { namespace channel { namespace socket { namespace asio {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;

AsioSocketChannel::AsioSocketChannel(Channel* parent,
                                     ChannelFactory* factory,
                                     ChannelPipeline* pipeline,
                                     ChannelSink* sink,
                                     AsioServicePool::IOService& ioService,
                                     const boost::thread::id& id)
    : SocketChannel(parent, factory, pipeline, sink),

      threadId(id),
      ioService(ioService),
      tcpSocket(ioService.service()),
      isWriting(false),
      highWaterMarkCounter(0),
      config(tcpSocket),
      state(ST_CHANNEL_OPEN) {
    writeQueue.setChannel(*this);
    ChannelBufferFactory* bufferFactory = config.getBufferFactory();
    readBuffer = bufferFactory->getBuffer(bufferFactory->getDefaultOrder(),
                                          config.getChannelOwnBufferSize());
}

AsioSocketChannel::~AsioSocketChannel() {
}

const SocketAddress& AsioSocketChannel::getLocalAddress() const {
    if (localAddress.validated()) {
        return localAddress;
    }

    boost::system::error_code ec;
    boost::asio::ip::tcp::endpoint endpoint = tcpSocket.local_endpoint(ec);

    if (!ec) {
        localAddress = SocketAddress(
            SocketAddress::SmartPointer(new AsioTcpSocketAddressImpl(ioService, endpoint)));
    }
    return localAddress;
}

const SocketAddress& AsioSocketChannel::getRemoteAddress() const {
    if (remoteAddress.validated()) {
        return remoteAddress;
    }

    boost::system::error_code ec;
    boost::asio::ip::tcp::endpoint endpoint = tcpSocket.remote_endpoint(ec);

    if (!ec) {
        remoteAddress = SocketAddress(
            SocketAddress::SmartPointer(new AsioTcpSocketAddressImpl(ioService, endpoint)));
    }
    return remoteAddress;
}

int AsioSocketChannel::getInterestOps() const {
    if (!isOpen()) {
        return Channel::OP_WRITE;
    }

    int interestOps = getRawInterestOps();

    int writeBufferSize = writeQueue.getWriteBufferSize();
    if (writeBufferSize != 0) {
        if (highWaterMarkCounter > 0) {
            int lowWaterMark = config.getWriteBufferLowWaterMark();
            if (writeBufferSize >= lowWaterMark) {
                interestOps |= Channel::OP_WRITE;
            }
            else {
                interestOps &= ~Channel::OP_WRITE;
            }
        }
        else {
            int highWaterMark = config.getWriteBufferHighWaterMark();
            if (writeBufferSize >= highWaterMark) {
                interestOps |= Channel::OP_WRITE;
            }
            else {
                interestOps &= ~Channel::OP_WRITE;
            }
        }
    }
    else {
        interestOps &= ~Channel::OP_WRITE;
    }

    return interestOps;
}

ChannelFuturePtr AsioSocketChannel::write(const ChannelMessage& message,
                                          const SocketAddress& remoteAddress,
                                          bool  withFutrue) {
    if (!remoteAddress.validated() || remoteAddress == this->remoteAddress) {
        return write(message, withFutrue);
    }
    else {
        return getUnsupportedOperationFuture();
    }
}

ChannelFuturePtr AsioSocketChannel::write(const ChannelMessage& message,
                                          bool  withFutrue) {
    ChannelFuturePtr future;
    if (withFutrue) {
        future = Channels::future(*this);
    }

    if (boost::this_thread::get_id() == threadId) {
        pipeline->sendDownstream(
            DownstreamMessageEvent(*this, future, message, this->remoteAddress));
    }
    else {
        ioService.service().post(
            make_custom_alloc_handler(ipcWriteAllocator,
            boost::bind<void, ChannelPipeline, const MessageEvent&>(
                        &ChannelPipeline::sendDownstream,
                        pipeline,
                        CopyableDownstreamMessageEvent(*this, future, message, this->remoteAddress))));
    }

    return future;
}

void AsioSocketChannel::write(const MessageEvent& evt) {
    const ChannelFuturePtr& f = evt.getFuture();
    if (!isConnected()) {
        cleanUpWriteBuffer();

        if (f) {
            f->setFailure(ChannelException("Channel has been closed."));
        }
        return;
    }

    isWriting = true;
    AsioWriteRequest writeRequest(evt);

    writeQueue.offer(writeRequest, f);

    if (writeRequest.writeBufferSize == 0) {
        ioService.service().post(boost::bind(
            &AsioSocketChannel::handleWrite, this, boost::system::error_code(), 0));
        return;
    }

    if (writeRequest.hasBuffers()) {
        boost::asio::async_write(tcpSocket,
            writeRequest.gathring.buffers,
            make_custom_alloc_handler(writeAllocator,
                boost::bind(&AsioSocketChannel::handleWrite,
                            this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred)));
    }
    else {
        boost::asio::async_write(tcpSocket,
            boost::asio::const_buffers_1(writeRequest.buffer),
            make_custom_alloc_handler(writeAllocator,
                boost::bind(&AsioSocketChannel::handleWrite,
                            this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred)));
    }
}

cetty::channel::ChannelFuturePtr AsioSocketChannel::unbind() {
    ChannelFuturePtr future = Channels::future(*this);
    if (boost::this_thread::get_id() == threadId) {
        pipeline->sendDownstream(DownstreamChannelStateEvent(
                                    *this, future, ChannelState::BOUND));
    }
    else {
        ioService.service().post(
            make_custom_alloc_handler(ipcStateChangeAllocator,
                boost::bind<void, ChannelPipeline, const ChannelStateEvent&>(
                    &ChannelPipeline::sendDownstream,
                    pipeline,
                    CopyableDownstreamChannelStateEvent(
                        *this, future, ChannelState::BOUND))));
    }

    return future;
}

cetty::channel::ChannelFuturePtr AsioSocketChannel::close() {
    if (closeFuture->isDone()) {
        return closeFuture;
    }

    if (boost::this_thread::get_id() == threadId) {
        pipeline->sendDownstream(DownstreamChannelStateEvent(
                                    *this, closeFuture, ChannelState::OPEN));
    }
    else {
        ioService.service().post(
            make_custom_alloc_handler(ipcStateChangeAllocator,
                boost::bind<void, ChannelPipeline, const ChannelStateEvent&>(
                    &ChannelPipeline::sendDownstream,
                    pipeline,
                    CopyableDownstreamChannelStateEvent(
                        *this, closeFuture, ChannelState::OPEN))));
    }

    return closeFuture;
}

void AsioSocketChannel::close(const ChannelFuturePtr& future) {
    bool connected = isConnected();
    bool bound = isBound();

    if (!isOpen() || !tcpSocket.is_open()) {
        return;
    }

    // If throw an exception when closing tcp_socket, there is no ChannelClosed
    // event fired, all what you can do is insert a FutureListener to
    // ChannelCloseFuture.
    try {
        if (connected) {
            tcpSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        }
        tcpSocket.close();

        if (setClosed()) {
            future->setSuccess();
            if (connected) {
                Channels::fireChannelDisconnected(*this);
            }
            if (bound) {
                Channels::fireChannelUnbound(*this);
            }

            cleanUpWriteBuffer();
            Channels::fireChannelClosed(*this);
        }
        else {
            future->setSuccess();
        }
    }
    catch (const boost::system::system_error& error) {
        IOException e("closing tcp socket has thrown an exception", error.code().value());
        setClosed();
        future->setFailure(e);
        Channels::fireExceptionCaught(*this, e);
    }
}

cetty::channel::ChannelFuturePtr AsioSocketChannel::disconnect() {
    ChannelFuturePtr future = Channels::future(*this);

    if (boost::this_thread::get_id() == threadId) {
        pipeline->sendDownstream(DownstreamChannelStateEvent(
                                    *this, future, ChannelState::CONNECTED));
    }
    else {
        ioService.service().post(
            make_custom_alloc_handler(ipcStateChangeAllocator,
                boost::bind<void, ChannelPipeline, const ChannelStateEvent&>(
                &ChannelPipeline::sendDownstream,
                pipeline,
                CopyableDownstreamChannelStateEvent(
                    *this, future, ChannelState::CONNECTED))));
    }

    return future;
}

cetty::channel::ChannelFuturePtr AsioSocketChannel::setInterestOps(int interestOps) {
    interestOps = Channels::validateAndFilterDownstreamInteresOps(interestOps);
    ChannelFuturePtr future = Channels::future(*this);

    if (boost::this_thread::get_id() == threadId) {
        pipeline->sendDownstream(DownstreamChannelStateEvent(
            *this, future, ChannelState::INTEREST_OPS, boost::any(interestOps)));
    }
    else {
        ioService.service().post(
            make_custom_alloc_handler(ipcStateChangeAllocator,
                boost::bind<void, ChannelPipeline, const ChannelStateEvent&>(
                &ChannelPipeline::sendDownstream,
                pipeline,
                CopyableDownstreamChannelStateEvent(
                    *this, future, ChannelState::INTEREST_OPS, boost::any(interestOps)))));
    }

    return future;
}

void AsioSocketChannel::setInterestOps(const ChannelFuturePtr& future, int interestOps) {
    bool isOrgReadable = isReadable();

    // Override OP_WRITE flag - a user cannot change this flag.
    interestOps &= ~Channel::OP_WRITE;
    interestOps |= getRawInterestOps() & Channel::OP_WRITE;

    setRawInterestOpsNow(interestOps);

    bool isNowReadable = isReadable();
    bool changed = isOrgReadable != isNowReadable;

    if (changed && isNowReadable) {
        Array readerArray;
        readBuffer->writableBytes(readerArray);
        tcpSocket.async_read_some(
            boost::asio::buffer(readerArray.data(), readerArray.length()),
            boost::bind(&AsioSocketChannel::handleRead,
            this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }

    future->setSuccess();
    if (changed) {
        Channels::fireChannelInterestChanged(*this, interestOps);
    }
}

void AsioSocketChannel::handleRead(const boost::system::error_code& error,
                                   size_t bytes_transferred) {
    if (!error) {
        readBuffer->offsetWriterIndex(bytes_transferred);

        // Fire the event.
        pipeline->sendUpstream(UpstreamMessageEvent(*this, readBuffer, remoteAddress));
        //Channels::fireMessageReceived(*this, ChannelMessage(readBuffer));

        if (interestOps & OP_READ) { //readable
            Array readerBuffer;
            readBuffer->writableBytes(readerBuffer);
            tcpSocket.async_read_some(
                boost::asio::buffer(readerBuffer.data(), readerBuffer.length()),
                make_custom_alloc_handler(readAllocator,
                    boost::bind(&AsioSocketChannel::handleRead,
                                this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred)));
        }
    }
    else {
        close();
    }
}

void AsioSocketChannel::handleWrite(const boost::system::error_code& error,
                                    size_t bytes_transferred) {
    if (!error) {
        writeQueue.poll().setSuccess();

        pipeline->sendUpstream(DefaultWriteCompletionEvent(*this, bytes_transferred));
        //Channels::fireWriteComplete(*this, bytes_transferred);

        if (writeQueue.empty()) {
            isWriting = false;
        }
    }
    else {
        writeQueue.poll().setFailure(
            RuntimeException(std::string("write buffer failed, code=") +
                             Integer::toString(error.value())));
        close();
    }
}

void AsioSocketChannel::handleConnect(const boost::system::error_code& error,
                                      boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
                                      const ChannelFuturePtr& cf) {
    if (!error) {
        setConnected();
        Channels::fireChannelConnected(*this, remoteAddress);
        cf->setSuccess();

        if (isReadable()) {
            Array readerBuffer;
            readBuffer->writableBytes(readerBuffer);
            tcpSocket.async_read_some(
                boost::asio::buffer(readerBuffer.data(), readerBuffer.length()),
                make_custom_alloc_handler(readAllocator,
                    boost::bind(&AsioSocketChannel::handleRead,
                                this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred)));
        }
    }
    else if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator()) {
        tcpSocket.close();
        boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
        tcpSocket.async_connect(endpoint,
            boost::bind(&AsioSocketChannel::handleConnect,
                        this,
                        boost::asio::placeholders::error,
                        ++endpoint_iterator,
                        cf));
    }
    else {
        cf->setFailure(Exception("can't connect to server"));
        close(cf);
    }
}

void AsioSocketChannel::cleanUpWriteBuffer() {
    ChannelException cause;
    bool fireExceptionCaught = false;

    // Clean up the stale messages in the write buffer.
    if (!writeQueue.empty()) {
        if (isOpen()) {
            cause = ChannelException("Channel has not close yet.");
        }
        else {
            cause = ChannelException("Channel has closed.");
        }

        // last one in the writeBuffer should not been cleaned.
        // it is already sent asynchronously, will take care of itself.
        while (writeQueue.size() > 1) {
            writeQueue.poll().setFailure(cause);
            fireExceptionCaught = true;
        }
    }

    if (fireExceptionCaught) {
        Channels::fireExceptionCaught(*this, cause);
    }
}

void AsioSocketChannel::handleAtHighWaterMark() {
    ++highWaterMarkCounter;
    Channels::fireChannelInterestChanged(*this, getInterestOps());
}

void AsioSocketChannel::handleAtLowWaterMark() {
    --highWaterMarkCounter;
    if (isConnected()) {
        Channels::fireChannelInterestChanged(*this, getInterestOps());
    }
}

}}}}
