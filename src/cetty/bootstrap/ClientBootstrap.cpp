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

#include <boost/assert.hpp>

#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelFactory.h"
#include "cetty/channel/ChannelConfig.h"
#include "cetty/channel/ChannelPipelineFactory.h"
#include "cetty/channel/ChannelPipelineException.h"
#include "cetty/channel/socket/ClientSocketChannelFactory.h"
#include "cetty/bootstrap/ClientBootstrap.h"

#include "cetty/util/Exception.h"

namespace cetty { namespace bootstrap {

using namespace cetty::channel;
using namespace cetty::channel::socket;
using namespace cetty::util;

ChannelFuturePtr ClientBootstrap::connect() {
    const SocketAddress* remoteAddress = getTypedOption<SocketAddress>("remoteAddress");
    if (remoteAddress == NULL) {
        throw IllegalStateException("remoteAddress option is not set.");
    }
    return connect(*remoteAddress);
}

ChannelFuturePtr ClientBootstrap::connect(const SocketAddress& remoteAddress) {
    const SocketAddress* localAddress =
        getTypedOption<SocketAddress>("localAddress");

    if (localAddress) {
        return connect(remoteAddress, *localAddress);
    }

    return connect(remoteAddress, SocketAddress::NULL_ADDRESS);
}

ChannelFuturePtr ClientBootstrap::connect(const SocketAddress& remoteAddress, const SocketAddress& localAddress) {
    ChannelPipeline* pipeline = NULL;
    Channel* ch = NULL;

    // FIXME:
    // for some reason, the pointer address of the impl int the remoteAddress
    // will be changed after getFactory()->newChannel(pipeline) under msvc8.
    SocketAddress remote = remoteAddress;
    SocketAddress local  = localAddress;

    try {
        pipeline = getPipelineFactory()->getPipeline();
    }
    catch (const Exception& e) {
        throw ChannelPipelineException("Failed to initialize a pipeline.", e);
    }
    catch (...) {
        throw ChannelPipelineException("Failed to initialize a pipeline.");
    }

    try {
        const ChannelFactoryPtr& factory = getFactory();
        ClientSocketChannelFactoryPtr clientFactory =
            boost::dynamic_pointer_cast<ClientSocketChannelFactory>(factory);
        if (clientFactory) {
            clientFactory->setIpProtocolVersion(remoteAddress.family());
        }

    	ch = getFactory()->newChannel(pipeline);
    }
    catch (const ChannelException& e) {
        throw ChannelPipelineException("Failed to create a new channel.", e);
    }

    // Set the options.
    ch->getConfig().setOptions(getOptions());

    // Bind.
    if (local.validated()) {
        ch->bind(local);
    }

    // Connect.
    ChannelFuturePtr future = ch->connect(remote);

    return future;
}

}}