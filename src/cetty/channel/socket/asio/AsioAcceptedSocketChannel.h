#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOACCEPTEDSOCKETCHANNEL_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOACCEPTEDSOCKETCHANNEL_H

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

#include "cetty/channel/socket/asio/AsioSocketChannel.h"

namespace cetty { namespace channel  { namespace socket { namespace asio {

class AsioAcceptedSocketChannel : public AsioSocketChannel {
public:
    AsioAcceptedSocketChannel(
        Channel* parent,
        ChannelFactory* factory,
        ChannelPipeline* pipeline,
        ChannelSink* sink,
        AsioServicePool::IOService& ioService,
        const boost::thread::id& id)
            : AsioSocketChannel(parent, factory, pipeline, sink, ioService, id) {
    }

    virtual ~AsioAcceptedSocketChannel() {}

    bool start() {
        Channels::fireChannelOpen(*this);

        const SocketAddress& localAddress = AsioSocketChannel::getLocalAddress();
        if (!localAddress.validated()) {
            // logging
            return false;
        }
        Channels::fireChannelBound(*this, localAddress);

        const SocketAddress& remoteAddress = AsioSocketChannel::getRemoteAddress();
        if (!remoteAddress.validated()) {
            // logging
            return false;
        }

        AsioSocketChannel::setConnected();
        Channels::fireChannelConnected(*this, remoteAddress);

        if (isReadable()) {
            Array readerBuffer;
            readBuffer->writableBytes(readerBuffer);
            tcpSocket.async_read_some(
                boost::asio::buffer(readerBuffer.data(), readerBuffer.length()),
                    boost::bind(&AsioSocketChannel::handleRead,
                                this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));
        }

        return true;
    }
};

}}}}


#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOACCEPTEDSOCKETCHANNEL_H)
