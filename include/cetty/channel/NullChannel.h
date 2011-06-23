#if !defined(CETTY_CHANNEL_NULLCHANNEL_H)
#define CETTY_CHANNEL_NULLCHANNEL_H

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

#include <string>

#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelFuture.h"
#include "cetty/util/Integer.h"

namespace cetty { namespace channel {

class SocketAddress;

class ChannelFactory;
class ChannelConfig;
class ChannelPipeline;
class ChannelFuture;

class ChannelMessage;

using namespace cetty::util;

class NullChannel : public Channel {
public:
    NullChannel();
    virtual ~NullChannel() {}

    virtual Integer getId() const;
    virtual int hashCode() const;

    virtual Channel* getParent() const { return NULL; }

    virtual ChannelFactory&  getFactory() const;
    virtual ChannelPipeline& getPipeline() const ;

    virtual ChannelConfig& getConfig();
    virtual const ChannelConfig& getConfig() const;

    virtual bool isOpen() const { return false; }
    virtual bool isBound() const { return false; }
    virtual bool isConnected() const { return false; }

    virtual const SocketAddress& getLocalAddress() const;
    virtual const SocketAddress& getRemoteAddress() const;

    virtual ChannelFuturePtr write(const ChannelMessage& message,
                                   bool  withFuture = true);
    virtual ChannelFuturePtr write(const ChannelMessage& message,
                                   const SocketAddress& remoteAddress,
                                   bool  withFuture = true);

    virtual ChannelFuturePtr bind(const SocketAddress& localAddress);
    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress);
    virtual ChannelFuturePtr disconnect();
    virtual ChannelFuturePtr unbind();
    virtual ChannelFuturePtr close();

    virtual ChannelFuturePtr& getCloseFuture();
    virtual ChannelFuturePtr& getSucceededFuture();

    virtual int  getInterestOps() const { return OP_NONE; }
    virtual bool isReadable() const { return false; }
    virtual bool isWritable() const { return false; }

    virtual ChannelFuturePtr setInterestOps(int interestOps);
    virtual ChannelFuturePtr setReadable(bool readable);

    virtual std::string toString() const { return "NullChannle"; }

public:
    static Channel& getInstance() { return nullChannel; }

private:
    static ChannelFuturePtr failedFuture;
    static NullChannel nullChannel;
};

}}

#endif //#if !defined(CETTY_CHANNEL_NULLCHANNEL_H)
