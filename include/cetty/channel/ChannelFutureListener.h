#if !defined(CETTY_CHANNEL_CHANNELFUTURELISTENER_H)
#define CETTY_CHANNEL_CHANNELFUTURELISTENER_H

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

#include "cetty/util/EventListener.h"
#include "cetty/channel/ChannelFuture.h"

namespace cetty { namespace channel {

class ChannelFuture;

/**
 * Listens to the result of a {@link ChannelFuture}.  The result of the
 * asynchronous {@link Channel} I/O operation is notified once this listener
 * is added by calling {@link ChannelFuture#addListener(ChannelFutureListener)}.
 *
 * <h3>Return the control to the caller quickly</h3>
 *
 * {@link #operationComplete(ChannelFuture)} is directly called by an I/O
 * thread.  Therefore, performing a time consuming task or a blocking operation
 * in the handler method can cause an unexpected pause during I/O.  If you need
 * to perform a blocking operation on I/O completion, try to execute the
 * operation in a different thread using a thread pool.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */
class ChannelFutureListener : public cetty::util::EventListener {
public:
    virtual ~ChannelFutureListener() {}

    /**
     * A {@link ChannelFutureListener} that closes the {@link Channel} which is
     * associated with the specified {@link ChannelFuture}.
     */
    static ChannelFutureListener* CLOSE;

    /**
     * A {@link ChannelFutureListener} that closes the {@link Channel} when the
     * operation ended up with a failure or cancellation rather than a success.
     */
    static ChannelFutureListener* CLOSE_ON_FAILURE;

    /**
     * Invoked when the I/O operation associated with the
     * {@link ChannelFuture const ChannelFuturePtr&} has been completed.
     *
     * @param future  the source {@link ChannelFuture} which called this
     *                callback
     */
    virtual void operationComplete(const ChannelFuturePtr& future) = 0;
};

}}


#endif //#if !defined(CETTY_CHANNEL_CHANNELFUTURELISTENER_H)
