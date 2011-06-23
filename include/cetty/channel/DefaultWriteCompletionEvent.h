#if !defined(CETTY_CHANNEL_DEFAULTWRITECOMPLETIONEVENT_H)
#define CETTY_CHANNEL_DEFAULTWRITECOMPLETIONEVENT_H

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

#include "cetty/channel/WriteCompletionEvent.h"
#include "cetty/util/Exception.h"

namespace cetty { namespace channel {

using namespace cetty::util;

/**
 * The default {@link WriteCompletionEvent} implementation.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2210 $, $Date: 2010-03-04 08:11:39 +0900 (Thu, 04 Mar 2010) $
 */

class DefaultWriteCompletionEvent : public WriteCompletionEvent {
public:
	virtual ~DefaultWriteCompletionEvent() {}
	
    /**
     * Creates a new instance.
     */
    DefaultWriteCompletionEvent(Channel& channel, int writtenAmount)
        : channel(channel),
          writtenAmount(writtenAmount) {
        if (writtenAmount <= 0) {
            throw InvalidArgumentException("writtenAmount must be a positive integer: ");
        }
    }

    Channel& getChannel() const {
        return channel;
    }

    const ChannelFuturePtr& getFuture() const {
        return channel.getSucceededFuture();
    }

    int getWrittenAmount() const {
        return writtenAmount;
    }

    std::string toString() const {
        char buf[512];
        sprintf(buf, "%s WRITTEN_AMOUNT: %d",
            getChannel().toString().c_str(), getWrittenAmount());
        
        return buf;
    }

private:
	Channel& channel;
	int writtenAmount;
};

}}

#endif //#if !defined(CETTY_CHANNEL_DEFAULTWRITECOMPLETIONEVENT_H)
