#if !defined(CETTY_CHANNEL_COMPLETECHANNELFUTURE_H)
#define CETTY_CHANNEL_COMPLETECHANNELFUTURE_H

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

#include <boost/thread.hpp>
#include "cetty/channel/ChannelFuture.h"
#include "cetty/channel/ChannelFutureListener.h"
#include "cetty/util/Exception.h"

namespace cetty { namespace logging {
class InternalLogger;
}}

namespace cetty { namespace channel {

using namespace ::cetty::util;
using namespace ::cetty::logging;

/**
 * A skeletal {@link ChannelFuture} implementation which represents a
 * {@link ChannelFuture} which has been completed already.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2192 $, $Date: 2010-02-19 18:58:38 +0900 (Fri, 19 Feb 2010) $
 */

class CompleteChannelFuture : public ChannelFuture {
public:
    virtual ~CompleteChannelFuture() {}

    virtual void setListener(const ListenerFunction& listener);

    virtual void addListener(ChannelFutureListener* listener);

    virtual void removeListener(const ChannelFutureListener* listener) {
        // NOOP
    }

    virtual ChannelFuture& await() {
        if (boost::this_thread::interruption_requested()) {
            throw InterruptedException();
        }
        return *this;
    }

    virtual bool await(boost::int64_t timeout, const TimeUnit& unit) {
        if (boost::this_thread::interruption_requested()) {
            throw InterruptedException();
        }
        return true;
    }

    virtual bool await(boost::int64_t timeoutMillis) {
        if (boost::this_thread::interruption_requested()) {
            throw InterruptedException();
        }
        return true;
    }

    virtual ChannelFuture& awaitUninterruptibly() {
        return *this;
    }

    virtual bool awaitUninterruptibly(boost::int64_t timeout, const TimeUnit& unit) {
        return true;
    }

    virtual bool awaitUninterruptibly(boost::int64_t timeoutMillis) {
        return true;
    }

    virtual Channel& getChannel() const {
        return *channel;
    }

    virtual bool isDone() const {
        return true;
    }

    virtual bool setProgress(int amount, int current, int total) {
        return false;
    }

    virtual bool setFailure(const Exception& cause) {
        return false;
    }

    virtual bool setSuccess() {
        return false;
    }

    virtual bool cancel() {
        return false;
    }

    virtual bool isCancelled() const {
        return false;
    }

protected:

    /**
     * Creates a new instance.
     *
     * @param channel the {@link Channel} associated with this future
     */
	CompleteChannelFuture(Channel& channel) : channel(&channel) {}

private:
    static InternalLogger* logger;
    Channel* channel;
};

}}


#endif //#if !defined(CETTY_CHANNEL_COMPLETECHANNELFUTURE_H)
