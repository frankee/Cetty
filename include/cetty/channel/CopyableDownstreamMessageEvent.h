#if !defined(CETTY_CHANNEL_COPYABLEDOWNSTREAMMESSAGEEVENT_H)
#define CETTY_CHANNEL_COPYABLEDOWNSTREAMMESSAGEEVENT_H

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

#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelFuture.h"
#include "cetty/channel/ChannelMessage.h"

#include "cetty/channel/MessageEvent.h"


namespace cetty { namespace channel {

/**
 * The default downstream {@link MessageEvent} implementation.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class CopyableDownstreamMessageEvent : public MessageEvent {
public:
    /**
     * Creates a new instance.
     */
    CopyableDownstreamMessageEvent(Channel& channel,
                                   const ChannelFuturePtr& future,
                                   const ChannelMessage& message,
                                   const SocketAddress& remoteAddress)
        : channel(channel), future(future), message(message), remoteAddress(remoteAddress) {
    }

    CopyableDownstreamMessageEvent(const CopyableDownstreamMessageEvent& evt)
        : channel(evt.channel),
          future(evt.future),
          message(evt.message),
          remoteAddress(evt.remoteAddress) {
    }

    CopyableDownstreamMessageEvent(const DownstreamMessageEvent& evt)
        : channel(evt.getChannel()),
          future(evt.getFuture()),
          message(evt.getMessage()),
          remoteAddress(evt.getRemoteAddress()) {
    }

    virtual ~CopyableDownstreamMessageEvent() {}

    Channel& getChannel() const {
        return this->channel;
    }

    const ChannelFuturePtr& getFuture() const {
        return this->future;
    }

    const ChannelMessage& getMessage() const {
        return this->message;
    }

    const SocketAddress& getRemoteAddress() const {
        return this->remoteAddress;
    }

    std::string toString() const {
        std::string buf = this->channel.toString();
        buf.append(" WRITE: ");
        buf.append(message.toString());

        if (this->remoteAddress != this->channel.getRemoteAddress()) {
            buf.append(" to ");
            buf.append(getRemoteAddress().toString());
        }

        return buf;
    }

private:
    Channel& channel;
    ChannelFuturePtr future;
    ChannelMessage   message;
    const SocketAddress& remoteAddress;
};

}}

#endif //#if !defined(CETTY_CHANNEL_COPYABLEDOWNSTREAMMESSAGEEVENT_H)
