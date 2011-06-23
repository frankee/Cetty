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

#include "cetty/bootstrap/ConnectionlessBootstrap.h"
#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelConfig.h"
#include "cetty/channel/ChannelFuture.h"
#include "cetty/channel/ChannelPipeline.h"
#include "cetty/channel/ChannelException.h"
#include "cetty/channel/ChannelPipelineException.h"
#include "cetty/channel/socket/DatagramChannelFactory.h"

#include "cetty/util/Exception.h"

namespace cetty { namespace bootstrap {

using namespace cetty::channel;
using namespace cetty::channel::socket;
using namespace cetty::util;

Channel* ConnectionlessBootstrap::bind() {
    const SocketAddress* localAddress = getTypedOption<SocketAddress>("localAddress");
    if (NULL == localAddress) return NULL;

    return bind(*localAddress);
}

Channel* ConnectionlessBootstrap::bind(const SocketAddress& localAddress) {
    ChannelPipeline* pipeline;
    try {
        pipeline = getPipelineFactory()->getPipeline();
    }
    catch (const Exception& e) {
        throw ChannelPipelineException("Failed to initialize a pipeline.", e);
    }

    const ChannelFactoryPtr& factory = getFactory();
    DatagramChannelFactoryPtr datagramFactory =
        boost::dynamic_pointer_cast<DatagramChannelFactory>(factory);
    if (datagramFactory) {
        datagramFactory->setIpProtocolVersion(localAddress.family());
    }

    Channel* ch = factory->newChannel(pipeline);

    // Apply options.
    ch->getConfig().setPipelineFactory(getPipelineFactory());
    ch->getConfig().setOptions(getOptions());

    // Bind
    ChannelFuturePtr future = ch->bind(localAddress);

    //Wait for the future.
    future->awaitUninterruptibly();
    if (!future->isSuccess()) {
        ch->close()->awaitUninterruptibly();

        std::string str("Failed to bind to: ");
        str.append(localAddress.toString());
        const Exception* exception = future->getCause();
        if (exception) {
            throw ChannelException(str, *exception);
        }
        else {
            throw ChannelException(str);
        }
    }

    return ch;
}

ChannelFuturePtr ConnectionlessBootstrap::connect() {
    const SocketAddress* remoteAddress = getTypedOption<SocketAddress>("remoteAddress");
    if (NULL == remoteAddress) ChannelFuturePtr();

    return connect(*remoteAddress);
}

ChannelFuturePtr ConnectionlessBootstrap::connect(const SocketAddress& remoteAddress) {
    if (remoteAddress == SocketAddress::NULL_ADDRESS) {
        throw InvalidArgumentException("remotedAddress");
    }

    const SocketAddress* localAddress = getTypedOption<SocketAddress>("localAddress");
    if (NULL == localAddress) {
        return connect(remoteAddress, SocketAddress::NULL_ADDRESS);
    }

    return connect(remoteAddress, *localAddress);
}

ChannelFuturePtr ConnectionlessBootstrap::connect(const SocketAddress& remoteAddress, const SocketAddress& localAddress) {
    ChannelPipeline* pipeline;
    try {
        pipeline = getPipelineFactory()->getPipeline();
    }
    catch (const Exception& e) {
        throw ChannelPipelineException("Failed to initialize a pipeline.", e);
    }
    
    const ChannelFactoryPtr& factory = getFactory();
    DatagramChannelFactoryPtr datagramFactory =
        boost::dynamic_pointer_cast<DatagramChannelFactory>(factory);
    if (datagramFactory) {
        datagramFactory->setIpProtocolVersion(remoteAddress.family());
    }

    Channel* ch = factory->newChannel(pipeline);

    // Set the options.
    ch->getConfig().setOptions(getOptions());

    // Bind.
    if (localAddress != SocketAddress::NULL_ADDRESS) {
        ch->bind(localAddress);
    }

    // Connect.
    return ch->connect(remoteAddress);
}

}}