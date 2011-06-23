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

#include <boost/crc.hpp>

#include "cetty/channel/AbstractChannel.h"
#include "cetty/channel/ChannelState.h"
#include "cetty/channel/DownstreamMessageEvent.h"
#include "cetty/channel/DownstreamChannelStateEvent.h"

namespace cetty { namespace channel {

AbstractChannel::IdDeallocator AbstractChannel::ID_DEALLOCATOR;
AbstractChannel::ChannelMap AbstractChannel::allChannels;

int AbstractChannel::hashCode() const {
    boost::crc_32_type crc32;
    crc32.process_bytes((void const*)this, sizeof(this));
    return crc32.checksum();
}

std::string AbstractChannel::toString() const {
    char buf[512] = {0};

    bool connected = isConnected();
    if (connected && !strVal.empty()) {
        return strVal;
    }

    const SocketAddress& localAddress = getLocalAddress();
    const SocketAddress& remoteAddress = getRemoteAddress();

    if (remoteAddress.validated()) {
        if (NULL == getParent()) { // server channel or client channel
            sprintf(buf, "[id: 0x%08x, %s => %s]", getId().intValue(),
                localAddress.toString().c_str(),
                remoteAddress.toString().c_str());
        }
        else { // connection channel
            sprintf(buf, "[id: 0x%08x, %s => %s]", getId().intValue(),
                remoteAddress.toString().c_str(),
                localAddress.toString().c_str());
        }
    }
    else if (localAddress.validated()) {
        sprintf(buf, "[id: 0x%08x, %s]", getId().intValue(),
            localAddress.toString().c_str());
    }
    else {
        sprintf(buf, "[id: 0x%08x]", getId().intValue());
    }

    if (connected) {
        strVal = (const char*)buf;
    }
    else {
        strVal.empty();
    }

    return buf;
}

ChannelFuturePtr AbstractChannel::write(const ChannelMessage& message,
                                        bool withFuture /*= true*/) {
    return AbstractChannel::write(message, getRemoteAddress(), withFuture);
}

ChannelFuturePtr AbstractChannel::write(const ChannelMessage& message,
                                        const SocketAddress& remoteAddress,
                                        bool withFuture /*= true*/) {
    ChannelFuturePtr future;

    if (withFuture) {
        future = Channels::future(*this);
    }

    pipeline->sendDownstream(DownstreamMessageEvent(*this, future, message, remoteAddress));

    return future;
}

ChannelFuturePtr AbstractChannel::unbind() {
    ChannelFuturePtr future = Channels::future(*this);
    pipeline->sendDownstream(DownstreamChannelStateEvent(
        *this, future, ChannelState::BOUND));

    return future;
}

ChannelFuturePtr AbstractChannel::close() {
    if (closeFuture->isDone()) {
        return closeFuture;
    }

    pipeline->sendDownstream(DownstreamChannelStateEvent(
        *this, closeFuture, ChannelState::OPEN));

    return closeFuture;
}

ChannelFuturePtr AbstractChannel::disconnect() {
    ChannelFuturePtr future = Channels::future(*this);
    pipeline->sendDownstream(DownstreamChannelStateEvent(
        *this, future, ChannelState::CONNECTED));

    return future;
}

ChannelFuturePtr AbstractChannel::setInterestOps(int interestOps) {
    interestOps = Channels::validateAndFilterDownstreamInteresOps(interestOps);

    ChannelFuturePtr future = Channels::future(*this);
    pipeline->sendDownstream(DownstreamChannelStateEvent(
        *this, future, ChannelState::INTEREST_OPS, boost::any(interestOps)));
    return future;
}

}}