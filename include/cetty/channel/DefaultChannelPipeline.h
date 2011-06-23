#if !defined(CETTY_CHANNEL_DEFAULTCHANNELPIPELINE_H)
#define CETTY_CHANNEL_DEFAULTCHANNELPIPELINE_H

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

#include <map>
#include <string>
#include <boost/thread.hpp>

#include "cetty/channel/ChannelSink.h"
#include "cetty/channel/ChannelHandler.h"
#include "cetty/channel/ChannelPipeline.h"
#include "cetty/channel/ChannelHandlerContext.h"

#include "cetty/channel/ChannelEvent.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ChannelStateEvent.h"
#include "cetty/channel/ChildChannelStateEvent.h"
#include "cetty/channel/ExceptionEvent.h"
#include "cetty/channel/WriteCompletionEvent.h"

#include "cetty/logging/InternalLogger.h"
#include "cetty/logging/InternalLoggerFactory.h"

namespace cetty { namespace util {
class Exception;
}}

namespace cetty { namespace logging {
class InternalLogger;
}}

namespace cetty { namespace channel {

using namespace cetty::util;
using namespace cetty::logging;

class LifeCycleAwareChannelHandler;
class Channel;
class ChannelEvent;
class ChannelPipelineException;

/**
 * The default {@link ChannelPipeline} implementation.  It is recommended
 * to use {@link Channels#pipeline()} to create a new {@link ChannelPipeline}
 * instance rather than calling the constructor directly.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2119 $, $Date: 2010-02-01 20:46:09 +0900 (Mon, 01 Feb 2010) $
 *
 */

class DefaultChannelPipeline : public ChannelPipeline {
protected:
    friend class DefaultChannelHandlerContext;
    class DefaultChannelHandlerContext : public ChannelHandlerContext {
    public:
        friend class DefaultChannelPipeline;

        DefaultChannelHandlerContext(DefaultChannelPipeline& pipeline,
                                     const std::string& name,
                                     const ChannelHandlerPtr& handler,
                                     DefaultChannelHandlerContext* prev,
                                     DefaultChannelHandlerContext* next);

        virtual ~DefaultChannelHandlerContext() {}

        virtual Channel& getChannel() const {
            return getPipeline().getChannel();
        }

        virtual ChannelPipeline& getPipeline() const {
            return this->pipeline;
        }

        virtual bool canHandleDownstream() const {
            return this->canHandleDowns;
        }

        virtual bool canHandleUpstream() const {
            return this->canHandleUps;
        }

        virtual const ChannelHandlerPtr& getHandler() const {
            return this->handler;
        }

        virtual ChannelUpstreamHandler* getUpstreamHandler() const {
            return this->upstreamHandler;
        }

        virtual ChannelDownstreamHandler* getDownstreamHandler() const {
            return this->downstreamHandler;
        }

        virtual const std::string& getName() const {
            return this->name;
        }

        virtual void* getAttachment() {
            return this->attachment;
        }

        virtual const void* getAttachment() const {
            return this->attachment;
        }

        virtual void setAttachment(void* attachment) {
            this->attachment = attachment;
        }

        virtual void sendDownstream(const ChannelEvent& e);
        virtual void sendDownstream(const MessageEvent& e);
        virtual void sendDownstream(const ChannelStateEvent& e);

        virtual void sendUpstream(const ChannelEvent& e);
        virtual void sendUpstream(const MessageEvent& e);
        virtual void sendUpstream(const WriteCompletionEvent& e);
        virtual void sendUpstream(const ChannelStateEvent& e);
        virtual void sendUpstream(const ChildChannelStateEvent& e);
        virtual void sendUpstream(const ExceptionEvent& e);

    protected:
        DefaultChannelPipeline& pipeline;

        DefaultChannelHandlerContext* next;
        DefaultChannelHandlerContext* prev;

        DefaultChannelHandlerContext* nextUpstream;
        DefaultChannelHandlerContext* nextDownstream;

    private:
        bool canHandleUps;
        bool canHandleDowns;

        std::string name;

        ChannelHandlerPtr           handler;
        ChannelUpstreamHandler*     upstreamHandler;
        ChannelDownstreamHandler*   downstreamHandler;
        
        void* attachment;
    };

    class DiscardingChannelSink : public ChannelSink {
    public:
        DiscardingChannelSink() {}
        virtual ~DiscardingChannelSink() {}

        virtual void eventSunk(const ChannelPipeline& pipeline,
                               const ChannelEvent& e);

        virtual void writeRequested(const ChannelPipeline& pipeline,
                                    const MessageEvent& e);

        virtual void stateChangeRequested(const ChannelPipeline& pipeline,
                                          const ChannelStateEvent& e);

        virtual void exceptionCaught(const ChannelPipeline& pipeline,
                                     const ChannelEvent& e,
                                     const ChannelPipelineException& cause);
    };
    
public:
    /**
     * Creates a new empty pipeline.
     */
    DefaultChannelPipeline()
        : channel(NULL), sink(&discardingSink), head(NULL), tail(NULL),
          upstreamHead(NULL), downstreamHead(NULL) {
    }

    virtual ~DefaultChannelPipeline() {}

    virtual Channel& getChannel() const;
    virtual ChannelSink& getSink() const;

    virtual void attach(Channel* channel, ChannelSink* sink);

    virtual bool isAttached() const;

    virtual void addFirst(const std::string& name, const ChannelHandlerPtr& handler);
    virtual void addLast(const std::string& name, const ChannelHandlerPtr& handler);
    virtual void addBefore(const std::string& baseName, const std::string& name, const ChannelHandlerPtr& handler);
    virtual void addAfter(const std::string& baseName, const std::string& name, const ChannelHandlerPtr& handler);

    virtual void remove(const ChannelHandlerPtr& handler);
    virtual ChannelHandlerPtr remove(const std::string& name);
    virtual ChannelHandlerPtr removeFirst();
    virtual ChannelHandlerPtr removeLast();

    virtual void replace(const ChannelHandlerPtr& oldHandler, const std::string& newName, const ChannelHandlerPtr& newHandler);
    virtual ChannelHandlerPtr replace(const std::string& oldName, const std::string& newName, const ChannelHandlerPtr& newHandler);
    virtual ChannelHandlerPtr getFirst() const;
    virtual ChannelHandlerPtr getLast() const;
    virtual ChannelHandlerPtr get(const std::string& name) const;

    virtual ChannelHandlerContext* getContext(const std::string& name) const;
    virtual ChannelHandlerContext* getContext(const ChannelHandlerPtr& handler) const;

    virtual ChannelHandlers toMap() const;

    /**
     * Returns the {@link std::string} representation of this pipeline.
     */
    virtual std::string toString() const;

    virtual void sendUpstream(const ChannelEvent& e);
    virtual void sendUpstream(const MessageEvent& e);
    virtual void sendUpstream(const ExceptionEvent& e);
    virtual void sendUpstream(const WriteCompletionEvent& e);
    virtual void sendUpstream(const ChannelStateEvent& e);
    virtual void sendUpstream(const ChildChannelStateEvent& e);

    virtual void sendDownstream(const ChannelEvent& e);
    virtual void sendDownstream(const MessageEvent& e);
    virtual void sendDownstream(const ChannelStateEvent& e);

    virtual void notifyHandlerException(const ChannelEvent& evt, const Exception& e);

protected:
    void callBeforeAdd(ChannelHandlerContext* ctx);
    void callAfterAdd(ChannelHandlerContext* ctx);
    void callBeforeRemove(ChannelHandlerContext* ctx);
    void callAfterRemove(ChannelHandlerContext* ctx);

private:
    void init(const std::string& name, const ChannelHandlerPtr& handler);

    void checkDuplicateName(const std::string& name);

    DefaultChannelHandlerContext* getContextNoLock(const std::string& name) const;
    DefaultChannelHandlerContext* getContextNoLock(const ChannelHandlerPtr& handler) const;

    DefaultChannelHandlerContext* getContextOrDie(const std::string& name);
    DefaultChannelHandlerContext* getContextOrDie(const ChannelHandlerPtr& handler);

    ChannelHandlerPtr remove(DefaultChannelHandlerContext* ctx);
    ChannelHandlerPtr replace(DefaultChannelHandlerContext* ctx, const std::string& newName, const ChannelHandlerPtr& newHandler);

    // upstream & downstream list operators.
    void updateUpstreamList();
    void updateDownstreamList();

protected:
    static InternalLogger* logger;

protected:
    typedef std::map<std::string, DefaultChannelHandlerContext*> ContextMap;
    ContextMap name2ctx;
    
private:
    static DiscardingChannelSink discardingSink;

private:
    Channel* channel;
    ChannelSink* sink;

    mutable boost::recursive_mutex mutex;

    DefaultChannelHandlerContext* head;
    DefaultChannelHandlerContext* tail;

    DefaultChannelHandlerContext* upstreamHead; //< upstream single list.
    DefaultChannelHandlerContext* downstreamHead; //< downstream single list.
};

}}

#endif //#if !defined(CETTY_CHANNEL_DEFAULTCHANNELPIPELINE_H)
