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

#include "cetty/channel/NullChannel.h"
#include "cetty/channel/ChannelFactory.h"
#include "cetty/channel/DefaultChannelConfig.h"
#include "cetty/channel/DefaultChannelPipeline.h"
#include "cetty/channel/FailedChannelFuture.h"
#include "cetty/channel/SocketAddress.h"
#include "cetty/util/Exception.h"

namespace cetty { namespace channel {

using namespace cetty::util;

class NullChannelFactory : public ChannelFactory {
public:
    virtual Channel* newChannel(ChannelPipeline* pipeline) {
        return new NullChannel;
    }
    virtual void releaseExternalResources() {}
};

static const Exception exception("NullChannel");
static DefaultChannelConfig channelConfig;
static DefaultChannelPipeline channelPipeline;
static NullChannelFactory nullChannelFactory;

NullChannel::NullChannel() {
    if (!failedFuture) {
        failedFuture = ChannelFuturePtr(new FailedChannelFuture(*this, exception));
    }
}

cetty::util::Integer NullChannel::getId() const {
    return Integer();
}

int NullChannel::hashCode() const {
    return 0;
}

ChannelFactory& NullChannel::getFactory() const {
    return nullChannelFactory;
}

ChannelConfig& NullChannel::getConfig() {
    return channelConfig;
}

const ChannelConfig& NullChannel::getConfig() const {
    return channelConfig;
}

ChannelPipeline& NullChannel::getPipeline() const {
    return channelPipeline;
}

const SocketAddress& NullChannel::getLocalAddress() const {
    return SocketAddress::NULL_ADDRESS;
}

const SocketAddress& NullChannel::getRemoteAddress() const {
    return SocketAddress::NULL_ADDRESS;
}

cetty::channel::ChannelFuturePtr NullChannel::write(const ChannelMessage& message, bool withFuture /*= true*/) {
    return failedFuture;
}

cetty::channel::ChannelFuturePtr NullChannel::write(const ChannelMessage& message, const SocketAddress& remoteAddress, bool withFuture /*= true*/) {
    return failedFuture;
}

cetty::channel::ChannelFuturePtr NullChannel::bind(const SocketAddress& localAddress) {
    return failedFuture;
}

cetty::channel::ChannelFuturePtr NullChannel::connect(const SocketAddress& remoteAddress) {
    return failedFuture;
}

cetty::channel::ChannelFuturePtr NullChannel::disconnect() {
    return failedFuture;
}

cetty::channel::ChannelFuturePtr NullChannel::unbind() {
    return failedFuture;
}

cetty::channel::ChannelFuturePtr NullChannel::close() {
    return failedFuture;
}

ChannelFuturePtr& NullChannel::getCloseFuture() {
    return failedFuture;
}

ChannelFuturePtr& NullChannel::getSucceededFuture() {
    return failedFuture;
}

cetty::channel::ChannelFuturePtr NullChannel::setInterestOps(int interestOps) {
    return failedFuture;
}

cetty::channel::ChannelFuturePtr NullChannel::setReadable(bool readable) {
     return failedFuture;
}

cetty::channel::NullChannel NullChannel::nullChannel;

cetty::channel::ChannelFuturePtr NullChannel::failedFuture;



}}