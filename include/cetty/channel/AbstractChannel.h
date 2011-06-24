#if !defined(CETTY_CHANNEL_ABSTRACTCHANNEL_H)
#define CETTY_CHANNEL_ABSTRACTCHANNEL_H

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

#include <string>
#include <map>

#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelPipeline.h"
#include "cetty/channel/ChannelFutureListener.h"
#include "cetty/channel/ChannelSink.h"

#include "cetty/channel/SucceededChannelFuture.h"
#include "cetty/channel/DefaultChannelFuture.h"
#include "cetty/channel/FailedChannelFuture.h"

#include "cetty/channel/Channels.h"
#include "cetty/channel/SocketAddress.h"

#include "cetty/util/Integer.h"
#include "cetty/util/Exception.h"

namespace cetty { namespace channel {

using namespace cetty::util;

/**
 * A skeletal {@link Channel} implementation.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class AbstractChannel : public Channel {
public:
    virtual ~AbstractChannel() {
        if (pipeline) {
            delete pipeline;
        }
    }

    virtual Integer getId() const {
        return id;
    }

    virtual Channel* getParent() const {
        return parent;
    }

    virtual ChannelFactory& getFactory() const {
        return *factory;
    }

    virtual ChannelPipeline& getPipeline() const {
        return *pipeline;
    }

    /**
     * Returns the @link System#identityHashCode(Object) identity hash code@endlink
     * of this channel.
     */
    virtual int hashCode() const;

    /**
     * Returns <tt>true</tt> if and only if the specified object is identical
     * with this channel (i.e: <tt>this == o</tt>).
     */
    virtual bool equals(const Channel& c) const {
        return this == &c;
    }

    /**
     * Compares the @link #getId() ID@endlink of the two channels.
     */
    virtual int compareTo(const Channel& c) const {
        return getId() == c.getId();
    }

    virtual bool isOpen() const {
        return !closeFuture->isDone();
    }

    virtual ChannelFuturePtr bind(const SocketAddress& localAddress) {
        return Channels::bind(*this, localAddress);
    }

    virtual ChannelFuturePtr unbind();

    virtual ChannelFuturePtr close();

    virtual ChannelFuturePtr& getCloseFuture() {
        return this->closeFuture;
    }

    /**
     * Returns the cached {@link SucceededChannelFuture} instance.
     */
    virtual ChannelFuturePtr& getSucceededFuture() {
        return this->succeededFuture;
    }

    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress) {
        return Channels::connect(*this, remoteAddress);
    }

    virtual ChannelFuturePtr disconnect();

    virtual int getInterestOps() const {
        return this->interestOps;
    }

    virtual ChannelFuturePtr setInterestOps(int interestOps);

    virtual bool isReadable() const {
        return (getInterestOps() & OP_READ) != 0;
    }

    virtual bool isWritable() const {
        return (getInterestOps() & OP_WRITE) == 0;
    }

    virtual ChannelFuturePtr setReadable(bool readable) {
        if (readable) {
			return setInterestOps(getInterestOps() | OP_READ);
        }
        else {
            return setInterestOps(getInterestOps() & (~OP_READ));
        }
    }

    virtual ChannelFuturePtr write(const ChannelMessage& message,
                                   bool  withFuture = true);

    virtual ChannelFuturePtr write(const ChannelMessage& message,
                                   const SocketAddress& remoteAddress,
                                   bool  withFuture = true);

    /**
     * Returns the {@link std::string} representation of this channel.  The returned
     * string contains the @link #getId() ID}, {@link #getLocalAddress() local address},
     * and {@link #getRemoteAddress() remote address} of this channel for
     * easier identification.
     */
    std::string toString() const;

    //TODO: should use a concurrent hash map.
    typedef std::map<Integer, Channel*> ChannelMap;

protected:
    /**
     * Creates a new instance.
     *
     * @param parent
     *        the parent of this channel. <tt>NULL</tt> if there's no parent.
     * @param factory
     *        the factory which created this channel
     * @param pipeline
     *        the pipeline which is going to be attached to this channel
     * @param sink
     *        the sink which will receive downstream events from the pipeline
     *        and send upstream events to the pipeline
     */
    AbstractChannel(
        Channel* parent, ChannelFactory* factory,
        ChannelPipeline* pipeline, ChannelSink* sink)
            : parent(parent), factory(factory), pipeline(pipeline), interestOps(OP_READ) {
        BOOST_ASSERT(factory && pipeline && sink && "input must not to be NULL!");
        id = allocateId(this);
        init(pipeline, sink);
    }

    /**
     * (Internal use only) Creates a new temporary instance with the specified
     * ID.
     *
     * @param parent
     *        the parent of this channel. <tt>NULL</tt> if there's no parent.
     * @param factory
     *        the factory which created this channel
     * @param pipeline
     *        the pipeline which is going to be attached to this channel
     * @param sink
     *        the sink which will receive downstream events from the pipeline
     *        and send upstream events to the pipeline
     */
    AbstractChannel(
            Integer id,
            Channel* parent, ChannelFactory* factory,
            ChannelPipeline* pipeline, ChannelSink* sink)
            : id(id), parent(parent), factory(factory), pipeline(pipeline), interestOps(OP_READ) {
        BOOST_ASSERT(factory && pipeline && sink && "input must not to be NULL!");
        init(pipeline, sink);
    }

    /**
     * Marks this channel as closed.  This method is intended to be called by
     * an internal component - please do not call it unless you know what you
     * are doing.
     *
     * @return <tt>true</tt> if and only if this channel was not marked as
     *                      closed yet
     */
    virtual bool setClosed() {
        return boost::static_pointer_cast<ChannelCloseFuture>(closeFuture)->setClosed();
    }

    /**
     * Returns the {@link FailedChannelFuture} whose cause is an
     * {@link UnsupportedOperationException}.
     */
    virtual ChannelFuturePtr getUnsupportedOperationFuture() {
        return ChannelFuturePtr(
                  new FailedChannelFuture(*this, UnsupportedOperationException()));
    }

    /**
     * Sets the {@link #getInterestOps() interestOps} property of this channel
     * immediately.  This method is intended to be called by an internal
     * component - please do not call it unless you know what you are doing.
     */
    virtual void setInterestOpsNow(int interestOps) {
        this->interestOps = interestOps;
    }

private:
    class IdDeallocator : public ChannelFutureListener {
    public:
        IdDeallocator() {}
        virtual ~IdDeallocator() {}

        virtual void operationComplete(const ChannelFuturePtr& future) {
            if (!future) return;
            AbstractChannel::ChannelMap::iterator itr
                = AbstractChannel::allChannels.find(future->getChannel().getId());

            if (itr != AbstractChannel::allChannels.end()) {
                AbstractChannel::allChannels.erase(itr);
            }
        }

        virtual std::string toString() const { return "IdDeallocator"; }
    };

    class ChannelCloseFuture : public DefaultChannelFuture {
    public:
        ChannelCloseFuture(Channel& channel)
            :DefaultChannelFuture(channel, false) {

        }

        ~ChannelCloseFuture() {}

        bool setSuccess() {
            // User is not supposed to call this method - ignore silently.
            return false;
        }

        bool setFailure(const Exception& cause) {
            // User is not supposed to call this method - ignore silently.
            return false;
        }

        bool setClosed() {
            return DefaultChannelFuture::setSuccess();
        }
    };

    typedef boost::intrusive_ptr<ChannelCloseFuture> ChannelCloseFuturePtr;

    Integer allocateId(Channel* channel) {
        Integer id = Integer::valueOf(channel->hashCode());
        for (;;) {
            // Loop until a unique ID is acquired.
            // It should be found in one loop practically.
            if (allChannels.insert(std::make_pair<Integer, Channel*>(id, channel)).second) {
                // Successfully acquired.
                return id;
            }
            else {
                // Taken by other channel at almost the same moment.
                id += 1;
            }
        }
    }

private:
    void init(ChannelPipeline* pipeline, ChannelSink* sink) {
        pipeline->attach(this, sink);
        succeededFuture = ChannelFuturePtr(new SucceededChannelFuture(*this));
        closeFuture = ChannelCloseFuturePtr(new ChannelCloseFuture(*this));

        closeFuture->addListener(&ID_DEALLOCATOR);
    }

private:
    static ChannelMap allChannels;
	static IdDeallocator ID_DEALLOCATOR;

protected:
    Integer id;

    Channel* parent; // just reference, do not maintenance it's life cycle
    ChannelFactory*  factory; // just reference.
    ChannelPipeline* pipeline; // own pipeline, and maintenance life cycle.

    ChannelFuturePtr succeededFuture;
    ChannelFuturePtr closeFuture;

    int interestOps;

    /** Cache for the string representation of this channel */
    mutable std::string strVal;
};

}}

#endif //#if !defined(CETTY_CHANNEL_ABSTRACTCHANNEL_H)
