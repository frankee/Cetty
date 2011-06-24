#if !defined(CETTY_CHANNEL_DEFAULTCHANNELFUTURE_H)
#define CETTY_CHANNEL_DEFAULTCHANNELFUTURE_H

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

#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelFuture.h"
#include "cetty/channel/ChannelFutureListener.h"
#include "cetty/channel/ChannelFutureProgressListener.h"

#include "cetty/util/TimeUnit.h"
#include "cetty/util/Exception.h"

namespace cetty { namespace logging {
class InternalLogger;
}}

namespace cetty { namespace channel {

using namespace ::cetty::util;
using namespace ::cetty::logging;

/**
 * The default {@link ChannelFuture} implementation.  It is recommended to
 * use {@link Channels#future(Channel)} and {@link Channels#future(Channel, bool)}
 * to create a new {@link ChannelFuture} rather than calling the constructor
 * explicitly.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class DefaultChannelFuture : public ChannelFuture {
public:
    /**
     * Returns <tt>true</tt> if and only if the dead lock checker is enabled.
     */
    static bool isUseDeadLockChecker() {
        return useDeadLockChecker;
    }

    /**
     * Enables or disables the dead lock checker.  It is not recommended to
     * disable the dead lock checker.  Disable it at your own risk!
     */
    static void setUseDeadLockChecker(bool useDeadLockChecker);

public:
    /**
     * Creates a new instance.
     *
     * @param channel
     *        the {@link Channel} associated with this future
     * @param cancellable
     *        <tt>true</tt> if and only if this future can be canceled
     */
    DefaultChannelFuture(Channel& channel, bool cancellable);
    virtual ~DefaultChannelFuture();

	virtual Channel& getChannel() const {
        return *(this->channel);
    }

    virtual bool isDone() const;

    virtual bool isSuccess() const;

    virtual const Exception* getCause() const;

    virtual bool isCancelled() const;

    virtual void setListener(const ListenerFunction& listener);
    virtual void addListener(ChannelFutureListener* listener);
    virtual void removeListener(const ChannelFutureListener* listener);

    virtual ChannelFuture& await();
    virtual bool await(boost::int64_t timeout, const TimeUnit& unit);
    virtual bool await(boost::int64_t timeoutMillis);

    virtual ChannelFuture& awaitUninterruptibly();
    virtual bool awaitUninterruptibly(boost::int64_t timeout, const TimeUnit& unit);
    virtual bool awaitUninterruptibly(boost::int64_t timeoutMillis);

    virtual bool setSuccess();
    virtual bool setFailure(const Exception& cause);

    virtual bool cancel();

    virtual bool setProgress(int amount, int current, int total);

private:
    bool await0(boost::int64_t timeoutMillis, bool interruptable);

    void checkDeadLock();

    void notifyListeners();
    void notifyListener(const ListenerFunction& l);
    void notifyListener(ChannelFutureListener* l);
    void notifyProgressListener(ChannelFutureProgressListener* l,
                                int amount,
                                int current,
                                int total);

    boost::condition_variable& condition();

private:
    static InternalLogger *logger;
    static Exception CANCELLED;

    static bool useDeadLockChecker;
    static bool disabledDeadLockCheckerOnce;

private:
    bool cancellable;
    bool done;

    Channel* channel;
    
    ListenerFunction functionListener;
    ChannelFutureListener* firstListener;
	std::list<ChannelFutureListener*>* otherListeners;
	std::list<ChannelFutureProgressListener*>* progressListeners;
    
    Exception* cause;

    mutable boost::mutex mutex;
    boost::condition_variable* cond;

    int waiters;
};

}}


#endif //#if !defined(CETTY_CHANNEL_DEFAULTCHANNELFUTURE_H)
