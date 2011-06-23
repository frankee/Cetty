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

#include "cetty/channel/DefaultChannelPipeline.h"

#include "cetty/channel/ChannelPipelineException.h"
#include "cetty/channel/ChannelHandlerLifeCycleException.h"

#include "cetty/channel/ChannelUpstreamHandler.h"
#include "cetty/channel/ChannelDownstreamHandler.h"
#include "cetty/channel/LifeCycleAwareChannelHandler.h"

#include "cetty/util/Exception.h"

namespace cetty { namespace channel {

using namespace cetty::util;

DefaultChannelPipeline::DiscardingChannelSink DefaultChannelPipeline::discardingSink = DefaultChannelPipeline::DiscardingChannelSink();
InternalLogger *DefaultChannelPipeline::logger = InternalLoggerFactory::getInstance("DefaultChannelPipeline");

DefaultChannelPipeline::DefaultChannelHandlerContext::DefaultChannelHandlerContext(DefaultChannelPipeline& pipeline,
                                                                                   const std::string& name,
                                                                                   const ChannelHandlerPtr& handler,
                                                                                   DefaultChannelHandlerContext* prev,
                                                                                   DefaultChannelHandlerContext* next)
                        : pipeline(pipeline),
                          next(next),
                          prev(prev),
                          nextUpstream(NULL),
                          nextDownstream(NULL),
                          canHandleUps(false),
                          canHandleDowns(false),
                          name(name),
                          handler(handler) {
    upstreamHandler = dynamic_cast<ChannelUpstreamHandler*>(handler.get());
    if (upstreamHandler) {
        this->canHandleUps = true;
    }

    downstreamHandler = dynamic_cast<ChannelDownstreamHandler*>(handler.get());
    if (downstreamHandler) {
        this->canHandleDowns = true;
    }

    if (!canHandleUps && !canHandleDowns) {
        throw InvalidArgumentException(
            "handler must be either ChannelUpstreamHandler or ChannelDownstreamHandler.");
    }
}

void DefaultChannelPipeline::DefaultChannelHandlerContext::sendDownstream(const ChannelEvent& e) {
    try {
        if (nextDownstream) {
            nextDownstream->downstreamHandler->handleDownstream(*nextDownstream, e);
        }
        else {
            BOOST_ASSERT(pipeline.sink);
            pipeline.sink->eventSunk(pipeline, e);
        }
    }
    catch (const Exception& t) {
        pipeline.notifyHandlerException(e, t);
    }
    catch (const std::exception& t) {
        pipeline.notifyHandlerException(e, Exception(t.what()));
    }
    catch (...) {
        pipeline.notifyHandlerException(e, Exception("unknown exception."));
    }
}

void DefaultChannelPipeline::DefaultChannelHandlerContext::sendDownstream(const MessageEvent& e) {
    try {
        if (nextDownstream) {
            nextDownstream->downstreamHandler->writeRequested(*nextDownstream, e);
        }
        else {
            BOOST_ASSERT(pipeline.sink);
            pipeline.sink->writeRequested(pipeline, e);
        }
    }
    catch (const Exception& t) {
        pipeline.notifyHandlerException(e, t);
    }
    catch (const std::exception& t) {
        pipeline.notifyHandlerException(e, Exception(t.what()));
    }
    catch (...) {
        pipeline.notifyHandlerException(e, Exception("unknown exception."));
    }
}

void DefaultChannelPipeline::DefaultChannelHandlerContext::sendDownstream(const ChannelStateEvent& e) {
    try {
        if (nextDownstream) {
            nextDownstream->downstreamHandler->stateChangeRequested(*nextDownstream, e);
        }
        else {
            BOOST_ASSERT(pipeline.sink);
            pipeline.sink->stateChangeRequested(pipeline, e);
        }
    }
    catch (const Exception& t) {
        pipeline.notifyHandlerException(e, t);
    }
    catch (const std::exception& t) {
        pipeline.notifyHandlerException(e, Exception(t.what()));
    }
    catch (...) {
        pipeline.notifyHandlerException(e, Exception("unknown exception."));
    }
}

void DefaultChannelPipeline::DefaultChannelHandlerContext::sendUpstream(const ChannelEvent& e) {
    if (nextUpstream) {
        try {
            nextUpstream->upstreamHandler->handleUpstream(*nextUpstream, e);
        }
        catch (const Exception& t) {
            pipeline.notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            pipeline.notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            pipeline.notifyHandlerException(e, Exception("unknown exception."));
        }
    }
}

void DefaultChannelPipeline::DefaultChannelHandlerContext::sendUpstream(const MessageEvent& e) {
    if (nextUpstream) {
        try {
            nextUpstream->upstreamHandler->messageReceived(*nextUpstream, e);
        }
        catch (const Exception& t) {
            pipeline.notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            pipeline.notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            pipeline.notifyHandlerException(e, Exception("unknown exception."));
        }
    }
}

void DefaultChannelPipeline::DefaultChannelHandlerContext::sendUpstream(const WriteCompletionEvent& e) {
    if (nextUpstream) {
        try {
            nextUpstream->upstreamHandler->writeCompleted(*nextUpstream, e);
        }
        catch (const Exception& t) {
            pipeline.notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            pipeline.notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            pipeline.notifyHandlerException(e, Exception("unknown exception."));
        }
    }
}

void DefaultChannelPipeline::DefaultChannelHandlerContext::sendUpstream(const ChannelStateEvent& e) {
    if (nextUpstream) {
        try {
            nextUpstream->upstreamHandler->channelStateChanged(*nextUpstream, e);
        }
        catch (const Exception& t) {
            pipeline.notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            pipeline.notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            pipeline.notifyHandlerException(e, Exception("unknown exception."));
        }
    }
}

void DefaultChannelPipeline::DefaultChannelHandlerContext::sendUpstream(const ChildChannelStateEvent& e) {
    if (nextUpstream) {
        try {
            nextUpstream->upstreamHandler->childChannelStateChanged(*nextUpstream, e);
        }
        catch (const Exception& t) {
            pipeline.notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            pipeline.notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            pipeline.notifyHandlerException(e, Exception("unknown exception."));
        }
    }
}

void DefaultChannelPipeline::DefaultChannelHandlerContext::sendUpstream(const ExceptionEvent& e) {
    if (nextUpstream) {
        try {
            nextUpstream->upstreamHandler->exceptionCaught(*nextUpstream, e);
        }
        catch (const Exception& t) {
            pipeline.notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            pipeline.notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            pipeline.notifyHandlerException(e, Exception("unknown exception."));
        }
    }
}

void DefaultChannelPipeline::DiscardingChannelSink::eventSunk(const ChannelPipeline& pipeline, const ChannelEvent& e) {
    logger->warn(std::string("Not attached yet; discarding: ") + e.toString());
}

void DefaultChannelPipeline::DiscardingChannelSink::writeRequested(const ChannelPipeline& pipeline, const MessageEvent& e) {
    logger->warn(std::string("Not attached yet; discarding: ") + e.toString());
}

void DefaultChannelPipeline::DiscardingChannelSink::stateChangeRequested(const ChannelPipeline& pipeline, const ChannelStateEvent& e) {
    logger->warn(std::string("Not attached yet; discarding: ") + e.toString());
}

void DefaultChannelPipeline::DiscardingChannelSink::exceptionCaught(
                                                        const ChannelPipeline& pipeline,
                                                        const ChannelEvent& e,
                                                        const ChannelPipelineException& cause) {

    cause.rethrow();
}

Channel& DefaultChannelPipeline::getChannel() const {
    if (this->channel == NULL) {
        throw IllegalStateException("channel has not been attached");
    }

    return *(this->channel);
}

ChannelSink& DefaultChannelPipeline::getSink() const {
    if (this->sink == NULL) {
        return this->discardingSink;
    }
    return *(this->sink);
}

void DefaultChannelPipeline::attach(Channel* channel, ChannelSink* sink) {
    if (channel == NULL) {
        throw NullPointerException("channel");
    }
    if (sink == NULL) {
        throw NullPointerException("sink");
    }

    if (this->channel != NULL || this->sink != &discardingSink) {
        throw IllegalStateException("attached already");
    }

    this->channel = channel;
    this->sink = sink;
}

bool DefaultChannelPipeline::isAttached() const {
    return (sink != &discardingSink && sink != NULL);
}

void DefaultChannelPipeline::addFirst(const std::string& name,
                                      const ChannelHandlerPtr& handler) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    if (!handler) return;

    if (name2ctx.empty()) {
        init(name, handler);
    }
    else {
        checkDuplicateName(name);
        DefaultChannelHandlerContext* oldHead = this->head;
        DefaultChannelHandlerContext* newHead =
            new DefaultChannelHandlerContext(*this, name, handler, NULL, oldHead);

        callBeforeAdd(newHead);

        oldHead->prev = newHead;
        this->head = newHead;

        // for upstream and downstream single list.
        if (newHead->canHandleUps) {
            updateUpstreamList();
        }
        if (newHead->canHandleDowns) {
            updateDownstreamList();
        }

        name2ctx[name] = newHead;

        callAfterAdd(newHead);
    }
}

void DefaultChannelPipeline::addLast(const std::string& name,
                                     const ChannelHandlerPtr& handler) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);
    if (!handler) return;

    if (name2ctx.empty()) {
        init(name, handler);
    }
    else {
        checkDuplicateName(name);
        DefaultChannelHandlerContext* oldTail = this->tail;
        DefaultChannelHandlerContext* newTail =
            new DefaultChannelHandlerContext(*this, name, handler, oldTail, NULL);

        callBeforeAdd(newTail);

        oldTail->next = newTail;
        this->tail = newTail;

        // for upstream and downstream single list.
        if (newTail->canHandleUps) {
            updateUpstreamList();
        }
        if (newTail->canHandleDowns) {
            updateDownstreamList();
        }

        name2ctx[name] = newTail;

        callAfterAdd(newTail);
    }
}

void DefaultChannelPipeline::addBefore(const std::string& baseName,
                                       const std::string& name,
                                       const ChannelHandlerPtr& handler) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);
    if (!handler) return;

    DefaultChannelHandlerContext* ctx = getContextOrDie(baseName);
    if (ctx == this->head) {
        addFirst(name, handler);
    }
    else {
        checkDuplicateName(name);
        DefaultChannelHandlerContext* newCtx =
            new DefaultChannelHandlerContext(*this, name, handler, ctx->prev, ctx);

        callBeforeAdd(newCtx);

        ctx->prev->next = newCtx;
        ctx->prev = newCtx;

        // for upstream and downstream single list.
        if (newCtx->canHandleUps) {
            updateUpstreamList();
        }
        if (newCtx->canHandleDowns) {
            updateDownstreamList();
        }

        name2ctx[name] = newCtx;

        callAfterAdd(newCtx);
    }
}

void DefaultChannelPipeline::addAfter(const std::string& baseName,
                                      const std::string& name,
                                      const ChannelHandlerPtr& handler) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);
    if (!handler) return;

    DefaultChannelHandlerContext* ctx = getContextOrDie(baseName);
    if (ctx == this->tail) {
        addLast(name, handler);
    }
    else {
        checkDuplicateName(name);
        DefaultChannelHandlerContext* newCtx =
            new DefaultChannelHandlerContext(*this, name, handler, ctx, ctx->next);

        callBeforeAdd(newCtx);

        ctx->next->prev = newCtx;
        ctx->next = newCtx;

        // for upstream and downstream single list.
        if (newCtx->canHandleUps) {
            updateUpstreamList();
        }
        if (newCtx->canHandleDowns) {
            updateDownstreamList();
        }

        name2ctx[name] = newCtx;

        callAfterAdd(newCtx);
    }
}

void DefaultChannelPipeline::remove(const ChannelHandlerPtr& handler) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);
    remove(getContextOrDie(handler));
}

ChannelHandlerPtr
DefaultChannelPipeline::remove(const std::string& name) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);
    return remove(getContextOrDie(name));
}

ChannelHandlerPtr
DefaultChannelPipeline::remove(DefaultChannelHandlerContext* ctx) {
    if (head == tail) {
        head = tail = NULL;
        name2ctx.clear();
    }
    else if (ctx == head) {
        return removeFirst();
    }
    else if (ctx == tail) {
        return removeLast();
    }
    else {
        callBeforeRemove(ctx);

        DefaultChannelHandlerContext* prev = ctx->prev;
        DefaultChannelHandlerContext* next = ctx->next;
        prev->next = next;
        next->prev = prev;

        // for upstream and downstream single list.
        if (ctx->canHandleUps) {
            updateUpstreamList();
        }
        if (ctx->canHandleDowns) {
            updateDownstreamList();
        }

        name2ctx.erase(ctx->getName());

        callAfterRemove(ctx);
    }

    ChannelHandlerPtr ptr = ctx->getHandler();
    delete ctx;
    return ptr;
}

ChannelHandlerPtr DefaultChannelPipeline::removeFirst() {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    if (name2ctx.empty()) {
        throw RangeException("name2ctx is empty");
    }

    DefaultChannelHandlerContext* oldHead = head;
    if (oldHead == NULL) {
        throw RangeException("head is null");
    }

    callBeforeRemove(oldHead);

    if (oldHead->next == NULL) {
        head = tail = NULL;
        name2ctx.clear();
    }
    else {
        oldHead->next->prev = NULL;
        head = oldHead->next;
        name2ctx.erase(oldHead->getName());
    }

    // for upstream and downstream single list.
    if (oldHead->canHandleUps) {
        updateUpstreamList();
    }
    if (oldHead->canHandleDowns) {
        updateDownstreamList();
    }

    callAfterRemove(oldHead);

    ChannelHandlerPtr ptr = oldHead->getHandler();
    delete oldHead;
    return ptr;
}

ChannelHandlerPtr DefaultChannelPipeline::removeLast() {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    if (name2ctx.empty()) {
        throw RangeException("name2ctx is empty");
    }

    DefaultChannelHandlerContext* oldTail = tail;
    if (oldTail == NULL) {
        throw RangeException("head is null");
    }

    callBeforeRemove(oldTail);

    if (oldTail->prev == NULL) {
        this->head = this->tail = NULL;
        name2ctx.clear();
    }
    else {
        oldTail->prev->next = NULL;
        this->tail = oldTail->prev;
        name2ctx.erase(oldTail->getName());
    }

    // for upstream and downstream single list.
    if (oldTail->canHandleUps) {
        updateUpstreamList();
    }
    if (oldTail->canHandleDowns) {
        updateDownstreamList();
    }

    callAfterRemove(oldTail);

    ChannelHandlerPtr ptr = oldTail->getHandler();
    delete oldTail;
    return ptr;
}

void DefaultChannelPipeline::replace(const ChannelHandlerPtr& oldHandler,
                                     const std::string& newName,
                                     const ChannelHandlerPtr& newHandler) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);
    replace(getContextOrDie(oldHandler), newName, newHandler);
}

ChannelHandlerPtr
DefaultChannelPipeline::replace(const std::string& oldName,
                                const std::string& newName,
                                const ChannelHandlerPtr& newHandler) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);
    return replace(getContextOrDie(oldName), newName, newHandler);
}

ChannelHandlerPtr
DefaultChannelPipeline::replace(DefaultChannelHandlerContext* ctx,
                                const std::string& newName,
                                const ChannelHandlerPtr& newHandler) {
    ChannelHandlerPtr replacedHandle;
    if (!newHandler) return replacedHandle;

    if (ctx == head) {
        replacedHandle = removeFirst();
        addFirst(newName, newHandler);
    }
    else if (ctx == tail) {
        replacedHandle = removeLast();
        addLast(newName, newHandler);
    }
    else {
        bool sameName = (ctx->getName().compare(newName) == 0);
        if (!sameName) {
            checkDuplicateName(newName);
        }

        DefaultChannelHandlerContext* prev = ctx->prev;
        DefaultChannelHandlerContext* next = ctx->next;
        DefaultChannelHandlerContext* newCtx =
            new DefaultChannelHandlerContext(*this, newName, newHandler, prev, next);

        callBeforeRemove(ctx);
        callBeforeAdd(newCtx);

        prev->next = newCtx;
        next->prev = newCtx;

        // for upstream and downstream single list.
        if (newCtx->canHandleUps || ctx->canHandleUps) {
            updateUpstreamList();
        }
        if (newCtx->canHandleDowns || ctx->canHandleDowns) {
            updateDownstreamList();
        }

        if (!sameName) {
            name2ctx.erase(ctx->getName());
            name2ctx[newName] = newCtx;
        }

        ChannelHandlerLifeCycleException removeException;
        ChannelHandlerLifeCycleException addException;
        bool removed = false;
        try {
            callAfterRemove(ctx);
            removed = true;
        }
        catch (const ChannelHandlerLifeCycleException& e) {
            removeException = e;
        }

        bool added = false;
        try {
            callAfterAdd(newCtx);
            added = true;
        }
        catch (const ChannelHandlerLifeCycleException& e) {
            addException = e;
        }

        if (!removed || !added) {
            delete ctx;
        }

        if (!removed && !added) {
            logger->warn(removeException.what(), removeException);
            logger->warn(addException.what(), addException);
            throw ChannelHandlerLifeCycleException(
                std::string("Both ") +
                ctx->getHandler()->toString() +
                std::string(".afterRemove() and ") +
                newCtx->getHandler()->toString() +
                std::string(".afterAdd() failed; see logs."));
        }
        else if (!removed) {
            removeException.rethrow();
        }
        else if (!added) {
            addException.rethrow();
        }

        replacedHandle = ctx->getHandler();
    }

    delete ctx;
    return replacedHandle;
}

ChannelHandlerPtr DefaultChannelPipeline::getFirst() const {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    DefaultChannelHandlerContext* head = this->head;
    if (head == NULL) {
        return ChannelHandlerPtr();
    }
    return head->getHandler();
}

ChannelHandlerPtr DefaultChannelPipeline::getLast() const {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    DefaultChannelHandlerContext* tail = this->tail;
    if (tail == NULL) {
        return ChannelHandlerPtr();
    }
    return tail->getHandler();
}

ChannelHandlerPtr
DefaultChannelPipeline::get(const std::string& name) const {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    ContextMap::const_iterator itr = name2ctx.find(name);
    if (itr == name2ctx.end()) {
        return ChannelHandlerPtr();
    }

    return itr->second->getHandler();
}

ChannelHandlerContext*
DefaultChannelPipeline::getContext(const std::string& name) const {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);
    return getContextNoLock(name);
}

ChannelHandlerContext*
DefaultChannelPipeline::getContext(const ChannelHandlerPtr& handler) const {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);
    return getContextNoLock(handler);
}

ChannelPipeline::ChannelHandlers DefaultChannelPipeline::toMap() const {
    ChannelHandlers map;

    if (name2ctx.empty()) {
        return map;
    }

    DefaultChannelHandlerContext* ctx = head;
    for (;;) {
        map.push_back(std::make_pair(ctx->getName(), ctx->getHandler()));
        ctx = ctx->next;
        if (ctx == NULL) {
            break;
        }
    }
    return map;
}

std::string DefaultChannelPipeline::toString() const {
    std::string buf("DefaultChannelPipeline {");
    buf.reserve(1024);

    DefaultChannelHandlerContext* ctx = this->head;
    for (;;) {
        buf.append("(");
        buf.append(ctx->getName());
        buf.append(" = ");
        buf.append(ctx->getHandler()->toString());
        buf.append(")");

        ctx = ctx->next;
        if (ctx == NULL) {
            break;
        }
        buf.append(", ");
    }
    buf.append(" }");
    return buf;
}

void DefaultChannelPipeline::notifyHandlerException(const ChannelEvent& evt, const Exception& e) {
    const ExceptionEvent* exceptionEvt = dynamic_cast<const ExceptionEvent*>(&evt);
    if (exceptionEvt) {
        logger->warn(
                "An exception was thrown by a user handler \
                 while handling an exception event ( ... )", e);
        return;
    }

    ChannelPipelineException pe(e.getMessage(), e.getCode());
    try {
        sink->exceptionCaught(*this, evt, pe);
    }
    catch (const Exception& e1) {
        logger->warn("An exception was thrown by an exception handler.", e1);
    }
}

void DefaultChannelPipeline::callBeforeAdd(ChannelHandlerContext* ctx) {
    LifeCycleAwareChannelHandler* h =
        dynamic_cast<LifeCycleAwareChannelHandler*>(ctx->getHandler().get());

    if (!h) {
        return;
    }

    try {
        h->beforeAdd(*ctx);
    }
    catch (const Exception& e) {
        throw ChannelHandlerLifeCycleException(
            ".beforeAdd() has thrown an exception; not adding.", e);
    }
}

void DefaultChannelPipeline::callAfterAdd(ChannelHandlerContext* ctx) {
    LifeCycleAwareChannelHandlerPtr h =
        boost::dynamic_pointer_cast<LifeCycleAwareChannelHandler>(ctx->getHandler());

    if (!h) {
        return;
    }

    try {
        h->afterAdd(*ctx);
    }
    catch (const Exception& e) {
        logger->warn("call afterAdd has throw an exception, then try to remove", e);

        bool removed = false;
        try {
            remove((DefaultChannelHandlerContext*)ctx);
            removed = true;
        }
        catch (const Exception& e) {
            logger->warn(
                std::string("Failed to remove a handler: ").append(ctx->getName()), e);

            if (removed) {
                throw ChannelHandlerLifeCycleException(
                    ".afterAdd() has thrown an exception; removed.", e);
            }
            else {
                throw ChannelHandlerLifeCycleException(
                    ".afterAdd() has thrown an exception; also failed to remove.", e);
            }
        }
    }
}

void DefaultChannelPipeline::callBeforeRemove(ChannelHandlerContext* ctx) {
    LifeCycleAwareChannelHandlerPtr h =
        boost::dynamic_pointer_cast<LifeCycleAwareChannelHandler>(ctx->getHandler());

    if (!h) return;

    try {
        h->beforeRemove(*ctx);
    }
    catch (const Exception& e) {
        throw ChannelHandlerLifeCycleException(
            ".beforeRemove() has thrown an exception; not removing.", e);
    }
}

void DefaultChannelPipeline::callAfterRemove(ChannelHandlerContext* ctx) {
    LifeCycleAwareChannelHandlerPtr h =
        boost::dynamic_pointer_cast<LifeCycleAwareChannelHandler>(ctx->getHandler());

    if (!h) return;

    try {
        h->afterRemove(*ctx);
    }
    catch (const Exception& e) {
        throw ChannelHandlerLifeCycleException(
            ".afterRemove() has thrown an exception.", e);
    }
}

void DefaultChannelPipeline::init(const std::string& name, const ChannelHandlerPtr& handler) {
    DefaultChannelHandlerContext* ctx =
        new DefaultChannelHandlerContext(*this, name, handler, NULL, NULL);

    callBeforeAdd(ctx);

    this->head = this->tail = ctx;

    // for upstream and downstream single list.
    if (ctx->canHandleUps) {
        upstreamHead = ctx;
    }
    if (ctx->canHandleDowns) {
        downstreamHead = ctx;
    }

    name2ctx.clear();
    name2ctx[name] = ctx;

    callAfterAdd(ctx);
}

void DefaultChannelPipeline::checkDuplicateName(const std::string& name) {
    if (name2ctx.find(name) != name2ctx.end()) {
        throw InvalidArgumentException("Duplicate handler name.");
    }
}

DefaultChannelPipeline::DefaultChannelHandlerContext*
DefaultChannelPipeline::getContextOrDie(const std::string& name) {
    DefaultChannelHandlerContext* ctx = getContextNoLock(name);
    if (ctx == NULL) {
        throw RangeException(name);
    }
    else {
        return ctx;
    }
}

DefaultChannelPipeline::DefaultChannelHandlerContext*
DefaultChannelPipeline::getContextOrDie(const ChannelHandlerPtr& handler) {
    DefaultChannelHandlerContext* ctx = getContextNoLock(handler);
    if (ctx == NULL) {
        throw RangeException("found no handle context");
    }
    else {
        return ctx;
    }
}

DefaultChannelPipeline::DefaultChannelHandlerContext*
DefaultChannelPipeline::getContextNoLock(const ChannelHandlerPtr& handler) const {
    if (!handler) {
        throw NullPointerException("handler");
    }
    if (name2ctx.empty()) {
        return NULL;
    }

    DefaultChannelHandlerContext* ctx = head;
    for (;;) {
        if (ctx->getHandler() == handler) {
            return ctx;
        }

        ctx = ctx->next;
        if (ctx == NULL) {
            break;
        }
    }
    return NULL;
}

DefaultChannelPipeline::DefaultChannelHandlerContext*
DefaultChannelPipeline::getContextNoLock(const std::string& name) const {
    ContextMap::const_iterator itr = name2ctx.find(name);
    if (itr == name2ctx.end()) {
        return NULL;
    }

    return itr->second;
}


void DefaultChannelPipeline::sendUpstream(const ChannelEvent& e) {
    if (upstreamHead) {
        try {
            upstreamHead->upstreamHandler->handleUpstream(*upstreamHead, e);
        }
        catch (const Exception& t) {
            notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            notifyHandlerException(e, Exception("unknown exception."));
        }
    }
    else {
        logger->warn("The pipeline contains no upstream handlers, discarding...");
    }
}

void DefaultChannelPipeline::sendUpstream(const MessageEvent& e) {
    if (upstreamHead) {
        try {
            upstreamHead->upstreamHandler->messageReceived(*upstreamHead, e);
        }
        catch (const Exception& t) {
            notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            notifyHandlerException(e, Exception("unknown exception."));
        }
    }
    else {
        logger->warn("The pipeline contains no upstream handlers, discarding...");
    }
}

void DefaultChannelPipeline::sendUpstream(const ExceptionEvent& e) {
    if (upstreamHead) {
        try {
            upstreamHead->upstreamHandler->exceptionCaught(*upstreamHead, e);
        }
        catch (const Exception& t) {
            notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            notifyHandlerException(e, Exception("unknown exception."));
        }
    }
    else {
        logger->warn("The pipeline contains no upstream handlers, discarding...");
    }
}

void DefaultChannelPipeline::sendUpstream(const WriteCompletionEvent& e) {
    if (upstreamHead) {
        try {
            upstreamHead->upstreamHandler->writeCompleted(*upstreamHead, e);
        }
        catch (const Exception& t) {
            notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            notifyHandlerException(e, Exception("unknown exception."));
        }
    }
    else {
        logger->warn("The pipeline contains no upstream handlers, discarding...");
    }
}

void DefaultChannelPipeline::sendUpstream(const ChannelStateEvent& e) {
    if (upstreamHead) {
        try {
            upstreamHead->upstreamHandler->channelStateChanged(*upstreamHead, e);
        }
        catch (const Exception& t) {
            notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            notifyHandlerException(e, Exception("unknown exception."));
        }
    }
    else {
        logger->warn("The pipeline contains no upstream handlers, discarding...");
    }
}

void DefaultChannelPipeline::sendUpstream(const ChildChannelStateEvent& e) {
    if (upstreamHead) {
        try {
            upstreamHead->upstreamHandler->childChannelStateChanged(*upstreamHead, e);
        }
        catch (const Exception& t) {
            notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            notifyHandlerException(e, Exception("unknown exception."));
        }
    }
    else {
        logger->warn("The pipeline contains no upstream handlers, discarding...");
    }
}

void DefaultChannelPipeline::sendDownstream(const ChannelEvent& e) {
    try {
        if (downstreamHead) {
            downstreamHead->downstreamHandler->handleDownstream(*downstreamHead, e);
        }
        else {
            BOOST_ASSERT(sink);
            sink->eventSunk(*this, e);
        }
    }
    catch (const Exception& t) {
        notifyHandlerException(e, t);
    }
    catch (const std::exception& t) {
        notifyHandlerException(e, Exception(t.what()));
    }
    catch (...) {
        notifyHandlerException(e, Exception("unknown exception."));
    }
}

void DefaultChannelPipeline::sendDownstream(const MessageEvent& e) {
    try {
        if (downstreamHead) {
            downstreamHead->downstreamHandler->writeRequested(*downstreamHead, e);
        }
        else {
            BOOST_ASSERT(sink);
            sink->writeRequested(*this, e);
        }
    }
    catch (const Exception& t) {
        notifyHandlerException(e, t);
    }
    catch (const std::exception& t) {
        notifyHandlerException(e, Exception(t.what()));
    }
    catch (...) {
        notifyHandlerException(e, Exception("unknown exception."));
    }
}

void DefaultChannelPipeline::sendDownstream(const ChannelStateEvent& e) {
    try {
        if (downstreamHead) {
            downstreamHead->downstreamHandler->stateChangeRequested(*downstreamHead, e);
        }
        else {
            BOOST_ASSERT(sink);
            sink->stateChangeRequested(*this, e);
        }
    }
    catch (const Exception& t) {
        notifyHandlerException(e, t);
    }
    catch (const std::exception& t) {
        notifyHandlerException(e, Exception(t.what()));
    }
    catch (...) {
        notifyHandlerException(e, Exception("unknown exception."));
    }
}

void DefaultChannelPipeline::updateUpstreamList() {
    DefaultChannelHandlerContext* up = tail;
    upstreamHead = NULL;

    while (up) {
        if (up->canHandleUps) {
            if (upstreamHead) {
                up->nextUpstream = upstreamHead;
            }
            upstreamHead = up;
        }
        up = up->prev;
    }
}

void DefaultChannelPipeline::updateDownstreamList() {
    DefaultChannelHandlerContext* down = head;
    downstreamHead = NULL;

    while (down) {
        if (down->canHandleDowns) {
            if (downstreamHead) {
                down->nextDownstream = downstreamHead;
            }
            downstreamHead = down;
        }
        down = down->next;
    }
}

}}
