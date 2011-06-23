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

#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include "cetty/channel/DefaultChannelFuture.h"

#include "cetty/logging/InternalLogger.h"
#include "cetty/logging/InternalLoggerFactory.h"

#include "cetty/util/TimeUnit.h"
#include "cetty/util/Exception.h"

namespace cetty { namespace channel {

using namespace cetty::util;
using namespace cetty::logging;

InternalLogger *DefaultChannelFuture::logger =
                    InternalLoggerFactory::getInstance("DefaultChannelFuture");

bool DefaultChannelFuture::useDeadLockChecker = true;
bool DefaultChannelFuture::disabledDeadLockCheckerOnce = false;

Exception DefaultChannelFuture::CANCELLED("Future cancelled");

DefaultChannelFuture::DefaultChannelFuture(Channel& channel, bool cancellable)
                      : cancellable(cancellable),
                        done(false),
                        channel(&channel),
                        functionListener(0),
                        firstListener(NULL),
                        otherListeners(NULL),
                        progressListeners(NULL),
                        cause(NULL),
                        cond(NULL),
                        waiters(0){
}

DefaultChannelFuture::~DefaultChannelFuture() {
    if (otherListeners) {
        delete otherListeners;
    }

    if (progressListeners) {
        delete progressListeners;
    }

    if (cause && cause != &CANCELLED) {
        delete cause;
    }

    if (cond) {
        delete cond;
    }
}

void DefaultChannelFuture::setUseDeadLockChecker(bool useDeadLockChecker) {
    if (!useDeadLockChecker && !disabledDeadLockCheckerOnce) {
        disabledDeadLockCheckerOnce = true;
        logger->debug("The dead lock checker in ChannelFuture \
                      has been disabled as requested at your own risk.");
    }
    DefaultChannelFuture::useDeadLockChecker = useDeadLockChecker;
}

bool DefaultChannelFuture::isDone() const {
    boost::lock_guard<boost::mutex> guard(mutex);
    return done;
}

bool DefaultChannelFuture::isSuccess() const {
    boost::lock_guard<boost::mutex> guard(mutex);
    return done && cause == NULL;
}

const Exception* DefaultChannelFuture::getCause() const {
    boost::lock_guard<boost::mutex> guard(mutex);

    if (cause != &CANCELLED) {
        return cause;
    } else {
        return NULL;
    }
}

bool DefaultChannelFuture::isCancelled() const {
    boost::lock_guard<boost::mutex> guard(mutex);
    return cause == &CANCELLED;
}

void DefaultChannelFuture::setListener(const ListenerFunction& listener) {
    if (listener.empty()) {
        return;
    }
    bool notifyNow = false;

    {
        boost::lock_guard<boost::mutex> guard(mutex);

        if (done) {
            notifyNow = true;
        }
        else {
            functionListener = listener;
        }
    }

    if (notifyNow) {
        notifyListener(listener);
    }
}

void DefaultChannelFuture::addListener(ChannelFutureListener* listener) {
    if (listener == NULL) {
        return;
    }

    bool notifyNow = false;
    {
        boost::lock_guard<boost::mutex> guard(mutex);

        if (done) {
            notifyNow = true;
        }
        else {
            if (firstListener == NULL) {
                firstListener = listener;
            }
            else {
                if (otherListeners == NULL) {
                    otherListeners = new std::list<ChannelFutureListener*>();
                }
                otherListeners->push_back(listener);
            }

            ChannelFutureProgressListener* progressListener =
                dynamic_cast<ChannelFutureProgressListener*>(listener);
            if (progressListener) {
                if (progressListeners == NULL) {
                    progressListeners = new std::list<ChannelFutureProgressListener*>();
                }
                progressListeners->push_back(progressListener);
            }
        }
    }

    if (notifyNow) {
        notifyListener(listener);
    }
}

void DefaultChannelFuture::removeListener(const ChannelFutureListener* listener) {
    if (listener == NULL) {
        return;
    }

    boost::lock_guard<boost::mutex> guard(mutex);
    if (!done) {
        if (listener == firstListener) {
            if (otherListeners != NULL && !otherListeners->empty()) {
                firstListener = otherListeners->front();
                otherListeners->pop_front();
            }
            else {
                firstListener = NULL;
            }
        }
        else if (otherListeners != NULL) {
            otherListeners->remove((ChannelFutureListener* const)listener);
        }

        const ChannelFutureProgressListener* progressListener =
            dynamic_cast<const ChannelFutureProgressListener*>(listener);
        if (progressListener && progressListeners) {
            progressListeners->remove(
                (ChannelFutureProgressListener* const)progressListener);
        }
    }
}

ChannelFuture& DefaultChannelFuture::await() {
    if (boost::this_thread::interruption_requested()) {
        throw InterruptedException();
    }

    {
        boost::unique_lock<boost::mutex> lock(mutex);
        while (!done) {
            checkDeadLock();
            waiters++;
            try {
                condition().wait(lock);
            }
            catch(...) {
                waiters--;
            }
        }
    }
    return *this;
}

bool DefaultChannelFuture::await(boost::int64_t timeout, const TimeUnit& unit) {
    return await0(unit.toMillis(timeout), true);
}

bool DefaultChannelFuture::await(boost::int64_t timeoutMillis) {
    return await0(timeoutMillis, true);
}

ChannelFuture& DefaultChannelFuture::awaitUninterruptibly() {
    bool interrupted = false;
    {
        boost::unique_lock<boost::mutex> lock(mutex);
        while (!done) {
            checkDeadLock();
            waiters++;
            try {
                condition().wait(lock);
            }
            catch (const boost::thread_interrupted& e) {
                interrupted = true;
                waiters--;

                logger->warn("thread interrupted while awaiting");
            }
        }
    }

    if (interrupted) {
        //TODO must get
        //Thread.currentThread().interrupt();
    }
    return *this;
}

bool DefaultChannelFuture::awaitUninterruptibly(boost::int64_t timeout, const TimeUnit& unit) {
    try {
        return await0(unit.toMillis(timeout), false);
    }
    catch (const boost::thread_interrupted& e) {
        throw RuntimeException("thread interrupted.");
    }
}

bool DefaultChannelFuture::awaitUninterruptibly(boost::int64_t timeoutMillis) {
    try {
        return await0(timeoutMillis, false);
    }
    catch (const boost::thread_interrupted& e) {
        throw RuntimeException("thread interrupted.");
    }
}

bool DefaultChannelFuture::setSuccess() {
    {
        boost::lock_guard<boost::mutex> guard(mutex);

        // Allow only once.
        if (done) {
            return false;
        }

        done = true;
        if (waiters > 0) {
            cond->notify_all();
        }
    }

    notifyListeners();
    return true;
}

bool DefaultChannelFuture::setFailure(const Exception& cause) {
    {
        boost::lock_guard<boost::mutex> guard(mutex);

        // Allow only once.
        if (done) {
            return false;
        }

        if (this->cause && this->cause != &CANCELLED) {
            delete this->cause;
            this->cause = NULL;
        }
        this->cause = new Exception(cause);

        done = true;
        if (waiters > 0) {
            cond->notify_all();
        }
    }

    notifyListeners();
    return true;
}

bool DefaultChannelFuture::cancel() {
    if (!cancellable) {
        return false;
    }

    {
        boost::lock_guard<boost::mutex> guard(mutex);

        // Allow only once.
        if (done) {
            return false;
        }

        cause = &CANCELLED;
        done = true;
        if (waiters > 0) {
            cond->notify_all();
        }
    }

    notifyListeners();
    return true;
}

bool DefaultChannelFuture::setProgress(int amount, int current, int total) {
    std::list<ChannelFutureProgressListener*> tmplist;

    {
        boost::lock_guard<boost::mutex> guard(mutex);

        // Do not generate progress event after completion.
        if (done) {
            return false;
        }

        if (progressListeners == NULL || progressListeners->empty()) {
            // Nothing to notify - no need to create an empty array.
            return true;
        }

        tmplist = *progressListeners;
    }

    std::list<ChannelFutureProgressListener*>::iterator itr;
    for (itr = tmplist.begin(); itr != tmplist.end(); ++itr) {
        notifyProgressListener(*itr, amount, current, total);
    }

    return true;
}

bool DefaultChannelFuture::await0(boost::int64_t timeoutMillis, bool interruptable) {
    if (interruptable && boost::this_thread::interruption_requested()) {
        throw InterruptedException("");
    }

    boost::posix_time::time_duration duration =
        boost::posix_time::milliseconds(timeoutMillis);

    boost::posix_time::ptime expiredTime(
        timeoutMillis <= 0 ? boost::get_system_time()
                           : boost::get_system_time() + duration);

    bool interrupted = false;

    try {
        boost::unique_lock<boost::mutex> lock(mutex);
        if (done) {
            return done;
        }
        else if (timeoutMillis <= 0) {
            return done;
        }

        checkDeadLock();
        waiters++;
        try {
            for (;;) {
                try {
                    condition().timed_wait(lock, expiredTime);
                }
                catch (const boost::thread_interrupted& e) {
                    if (interruptable) {
                        throw InterruptedException("");
                    }
                    else {
                        interrupted = true;
                    }
                }

                if (done) {
                    return true;
                }
                else {
                    if (boost::get_system_time() >= expiredTime) {
                        return done;
                    }
                }
            }
        }
        catch (...) {
            waiters--;
        }
    }
    catch (...) {
        if (interrupted) {
            //Thread.currentThread().interrupt();
        }
    }
    return false;
}

void DefaultChannelFuture::checkDeadLock() {
//     if (isUseDeadLockChecker() && IoWorkerRunnable.IN_IO_THREAD.get()) {
//         throw IllegalStateException(
//                 "await*() in I/O thread causes a dead lock or "
//                 "sudden performance drop. Use addListener() instead or "
//                 "call await*() from a different thread.");
//     }
}

void DefaultChannelFuture::notifyListeners() {
    // This method doesn't need synchronization because:
    // 1) This method is always called after synchronized (this) block.
    //    Hence any listener list modification happens-before this method.
    // 2) This method is called only when 'done' is true.  Once 'done'
    //    becomes true, the listener list is never modified - see add/removeListener()
    if (firstListener != NULL) {
        notifyListener(firstListener);
        firstListener = NULL;

        if (otherListeners && !otherListeners->empty()) {
            std::list<ChannelFutureListener*>::iterator itr;
            for (itr = otherListeners->begin(); itr != otherListeners->end(); ++itr) {
                notifyListener(*itr);
            }
            otherListeners->empty();
        }
    }

    if (functionListener) {
        notifyListener(functionListener);
        functionListener = 0;
    }
}

void DefaultChannelFuture::notifyListener(ChannelFutureListener* l) {
    try {
        l->operationComplete(shared_from_this());
    }
    catch (const Exception& e) {
        logger->warn(
            "An exception was thrown by ChannelFutureListener .", e);
    }
}

void DefaultChannelFuture::notifyListener(const ListenerFunction& l) {
    try {
        l(shared_from_this());
    }
    catch (const Exception& e) {
        logger->warn(
            "An exception was thrown by ChannelFutureListener .", e);
    }
}

void DefaultChannelFuture::notifyProgressListener(ChannelFutureProgressListener* l, int amount, int current, int total) {
    if (NULL == l) return;

    try {
        l->operationProgressed(shared_from_this(), amount, current, total);
    }
    catch (const Exception& e) {
        logger->warn(
            std::string("An exception was thrown by ").append(l->toString()),
            e);
    }
}

boost::condition_variable& DefaultChannelFuture::condition() {
    if (!cond) {
        cond = new boost::condition_variable();
    }
    return *cond;
}



}}
